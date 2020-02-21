namespace NewStuff {
	Component* Component::create(std::string name, int entity) {
		Component* component = new Component;
		component->id = Component::next_id++;
		component->name = name;
		component->entity = entity;

		auto& c = *component;
		sol::protected_function on_component_created = Lua.state["on_component_created"];
		auto result = on_component_created(c);
		if (!result.valid()) {
			sol::error error = result;
			tdns_log.write("Failed to create component. Entity ID: " + std::to_string(entity) + ". Component name: " + name + ". Lua error:");
			tdns_log.write(error.what());
		}

		return component;
	}

	void Component::update(float dt) {
		sol::protected_function update_component = Lua.state["update_component"];
		auto result = update_component(id, dt);
		if (!result.valid()) {
			sol::error error = result;
			tdns_log.write("Component update failed!");
			tdns_log.write("Component name: " + name);
			tdns_log.write("Entity ID: " + std::to_string(entity));
			tdns_log.write(error.what());
		}
	}

	std::string Component::get_name() {
		return name;
	}
	
	int Component::get_entity() {
		return entity;
	}

	int Component::get_id() {
		return id;
	}

	Entity::Entity(std::string name, int id) {
		this->name = name;
		this->id = id;
	}

	int Entity::get_id() {
		return id;
	}

	std::string Entity::get_name() {
		return name;
	}

	void Entity::update(float dt) {
		sol::protected_function update_entity = Lua.state["update_entity"];
		auto result = update_entity(id, dt);
		if (!result.valid()) {
			sol::error error = result;
			tdns_log.write("Entity update failed. Entity name: " + name + "Entity ID: " + std::to_string(id) + ". Lua error:");
			tdns_log.write(error.what());
		}

		for (auto& [kind, component] : components) {
			component->update(dt);
		}
	}

	Component* Entity::add_component(std::string name) {
		auto c = Component::create(name, id);
		components[name] = c;
		return c;
	}

	Component* Entity::get_component(std::string name) {
		if (components.find(name) == components.end()) {
			tdns_log.write("Tried to get a component, but it didn't exist. Entity ID: " + std::to_string(id) + ". Component name: " + name);
		}
		return components[name];
	}

	Entity* EntityManager::get_entity(int id) {
		return entities[id].get();
	}

	bool EntityManager::has_entity(int id) {
		return entities.find(id) != entities.end();
	}

	EntityHandle EntityManager::create_entity(std::string name) {
		// Construct the entity
		auto inserted = entities.emplace(Entity::next_id, std::make_unique<Entity>(name, Entity::next_id));
		Entity::next_id++;
		auto it = inserted.first;
		Entity& entity = *it->second;

		// Add it to Lua
		sol::protected_function on_entity_created = Lua.state["on_entity_created"];
		auto result = on_entity_created(entity);
		if (!result.valid()) {
			sol::error error = result;
			tdns_log.write("Failed to create entity. Name was: " + name);
			tdns_log.write(error.what());
		}
	
		// Return a handle
		EntityHandle handle;
		handle.id = entity.id;
		handle.manager = this;
		return handle;
	}

	EntityHandle::operator bool() const {
		if (manager->has_entity(id)) return manager->get_entity(id);
		return false;
	}

	Entity* EntityHandle::operator->() const {
		return get();
	}

	Entity* EntityHandle::get() const {
		fox_assert(manager->has_entity(id));
		return manager->get_entity(id);
	}

	void Scene::add_entity(EntityHandle entity) {
		entities.push_back(entity);
	}

	void Scene::update(float dt) {
		for (auto entity : entities) {
			entity->update(dt);
		}
	}
	
	Scene* SceneManager::create_scene(std::string name) {
		auto scene = new Scene;
		scene->name = name;
		scenes[name] = scene;
		return scene;
	}

	void _RenderEngine::draw(Component* graphic, Component* position, EntityHandle entity, Render_Flags flags) {
		Render_Element info = { graphic, position, entity, flags };
		render_list.push_back(info);
	}
	
	void _RenderEngine::render_for_frame() {
		bind_sprite_buffers();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); // Verts always the same (a square)
		glEnableVertexAttribArray(0);

		Shader* shader = &textured_shader;
		shader->begin();
		shader->set_int("sampler", 0);

		// Algorithm:
		// Sort by Z-position (as if you were going to do the Painter's algorithm
		// Z positions are integral (since 2D), so just collect elements of each Z-type into collections
		// Sort these collections by Y-axis (so overlap works properly)
		auto sort_by_z = [](const Render_Element& a, const Render_Element& b) {
			float za = Lua.get_component(a.graphic->get_id())["z"];
			float zb = Lua.get_component(b.graphic->get_id())["z"];
			return za < zb;
		};
		sort(render_list.begin(), render_list.end(), sort_by_z);

	
		int zmax = std::numeric_limits<int>::min();
		std::vector<std::vector<Render_Element>> depth_sorted_render_elements;
		for (auto& render_element : render_list) {
			auto& [graphic, position, entity, flags] = render_element;
			int z = Lua.get_component(graphic->get_id())["z"];
			if (z > zmax) {
				std::vector<Render_Element> new_depth_level;
				depth_sorted_render_elements.push_back(new_depth_level);
				zmax = z;
			}
			depth_sorted_render_elements.back().push_back(render_element);
		}

		// Main render loop
		glm::vec2 camera_translation = magnitude_gl_from_screen(active_layer->camera.offset);
		for (auto& depth_level_render_elements : depth_sorted_render_elements) {
			auto sort_by_world_pos = [](const Render_Element& a, const Render_Element& b) {
				float wpa = Lua.get_component(a.position->get_id())["world"]["y"];
				float wpb = Lua.get_component(b.position->get_id())["world"]["y"];
				return wpa > wpb; 
			};
			stable_sort(depth_level_render_elements.begin(), depth_level_render_elements.end(), sort_by_world_pos);
		
			// Draw the correctly sorted elements for a depth level
			for (auto& [graphic, position, entity, flags] : depth_level_render_elements) {
				// Swap shader based on flags
				if (flags & Render_Flags::Highlighted) {
					if (shader != &highlighted_shader) {
						shader->end();
						shader = &highlighted_shader;
						shader->begin();
						shader->set_int("sampler", 0);
					}
				}
				else {
					if (shader != &textured_shader) {
						shader->end();
						shader = &textured_shader;
						shader->begin();
						shader->set_int("sampler", 0);
					}
				}

				Sprite* sprite = nullptr;//render_element.gc->get_current_frame();
				if (!sprite) {
					tdns_log.write("Trying to render, but sprite returned was invalid (nullptr). Sprite was: " + sprite->name);
					continue;
				}
				if (!sprite->is_initialized()) {
					tdns_log.write("Trying to render, but the sprite was uninitialized. Sprite was: " + sprite->name);
					continue;
				}
			
				sprite->atlas->bind();

				// Point the texture coordinates to this sprite's texcoords
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), sprite->tex_coord_offset);
				glEnableVertexAttribArray(1);
			
				SRT transform = SRT::no_transform();
				transform.scale = {
					Lua.get_component(graphic->get_id())["scale"]["x"],
					Lua.get_component(graphic->get_id())["scale"]["y"] };
				transform.translate = gl_from_screen({
						Lua.get_component(position->get_id())["world"]["x"],
						Lua.get_component(position->get_id())["world"]["y"] });
				transform.translate -= camera_translation;
				auto transform_mat = mat3_from_transform(transform);
				shader->set_mat3("transform", transform_mat);

				shader->check();
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}

		shader->end();
		render_list.clear();


		// Finally, render all the primitives on top of the screen in the order they were queued.
		for (auto& draw_func : primitives) {
			draw_func();
		}
		primitives.clear();
	}

	void draw_entity(EntityHandle entity, Render_Flags flags) {
		if (!entity) return;
		
		auto graphic = entity->get_component("Graphic");
		auto position = entity->get_component("Position");
		if (!graphic || !position) {
			tdns_log.write("Called draw_entity(), but entity was lacking component.");
			tdns_log.write("Entity name: " + entity->get_name());
			tdns_log.write("Entity ID: " + entity->get_id());
			return;
		}

		RenderEngine.draw(graphic, position, entity, flags);
	}
}
