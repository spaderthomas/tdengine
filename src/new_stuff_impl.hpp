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
		// Don't double add components
		if (components.find(name) != components.end()) {
			return components[name];
		}
		
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

	EntityManager& get_entity_manager() {
		static EntityManager manager;
		return manager;
	}

	void EntityManager::update(float dt) {
		for (auto& [id, entity] : entities) {
			entity->update(dt);
		}
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
		return handle;
	}

	EntityHandle::operator bool() const {
		auto& manager = get_entity_manager();
		if (manager.has_entity(id)) return manager.get_entity(id);
		return false;
	}

	Entity* EntityHandle::operator->() const {
		return get();
	}

	Entity* EntityHandle::get() const {
		auto& manager = get_entity_manager();
		fox_assert(manager.has_entity(id));
		return manager.get_entity(id);
	}

	void Animation::add_frames(std::vector<std::string>& frames_to_add) {
		for (auto& frame : frames_to_add) {
			this->add_frame(frame);
		}
	}
	
	void Animation::add_frame(std::string& frame) {
		Sprite* sprite = asset_table.get_asset<Sprite>(frame);
		if (!sprite) {
			tdns_log.write("Tried to add a frame to an animation, but couldn't find the sprite.");
			tdns_log.write("Requested sprite: " + frame);
			tdns_log.write("Animation name" + this->name);
			return;
		}
	
		frames.push_back(frame);
	}

	std::string Animation::get_frame(int frame) {
		return this->frames[frame];
	}
	
	SceneManager& get_scene_manager() {
		static SceneManager manager;
		return manager;
	}
	
	EntityHandle SceneManager::create_scene(std::string name) {
		auto& entity_manager = get_entity_manager();
		auto scene = entity_manager.create_entity(name);
		auto& scene_ref = *entity_manager.get_entity(scene.id);
			
		// Add it to Lua
		sol::protected_function on_scene_created = Lua.state["on_scene_created"];
		auto result = on_scene_created(scene_ref);
		if (!result.valid()) {
			sol::error error = result;
			tdns_log.write("Failed to create scene. Name was: " + name);
			tdns_log.write(error.what());
		}
	
		return scene;
	}

	EntityHandle SceneManager::get_scene(std::string name) {
		auto it = scenes.find(name);
		if (it == scenes.end()) {
			tdns_log.write("Tried to get scene: " + name + ", but it didn't exist.");
			return EntityHandle{-1};
		}

		return it->second;
	}
	EntityHandle SceneManager::add_entity(std::string scene, std::string entity) {
		// @spader 3/1/20: This function is worthless right now, but you're definitely going to want to do some
		// bookkeeping to map entities to scenes. So you'd do that here.
		auto& entity_manager = get_entity_manager();
		return entity_manager.create_entity(entity);
	}
	
	RenderEngine& get_render_engine() {
		static RenderEngine engine;
		return engine;
	}
	void RenderEngine::draw(EntityHandle entity, Render_Flags flags) {
		auto check_component_exists = [entity](auto component, std::string component_type) -> bool {
			if (component) return true;
			
			tdns_log.write("Called draw_entity(), but entity had no" + component_type + " component");
			tdns_log.write("Entity name: " + entity->get_name());
			tdns_log.write("Entity ID: " + entity->get_id());
			return false;
		};
		
		auto graphic = entity->get_component("Graphic");
		if (!check_component_exists(graphic, "Graphic")) return;
		auto position = entity->get_component("Position");
		if (!check_component_exists(position, "Position")) return;
		auto animation = entity->get_component("Animation");
		if (!check_component_exists(animation, "Animation")) return;

		Render_Element info = {
			graphic,
			position,
			animation,
			flags
		};
		render_list.push_back(info);
	}
	
	void RenderEngine::render() {
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
			int z = Lua.get_component(graphic->get_id())["layer"];
			if (z > zmax) {
				std::vector<Render_Element> new_depth_level;
				depth_sorted_render_elements.push_back(new_depth_level);
				zmax = z;
			}
			depth_sorted_render_elements.back().push_back(render_element);
		}

		// Main render loop
		glm::vec2 camera_translation = magnitude_gl_from_screen(camera_offset);
		for (auto& depth_level_render_elements : depth_sorted_render_elements) {
			auto sort_by_world_pos = [](const Render_Element& a, const Render_Element& b) {
				float wpa = Lua.get_component(a.position->get_id())["world"]["y"];
				float wpb = Lua.get_component(b.position->get_id())["world"]["y"];
				return wpa > wpb; 
			};
			stable_sort(depth_level_render_elements.begin(), depth_level_render_elements.end(), sort_by_world_pos);
		
			// Draw the correctly sorted elements for a depth level
			for (auto& [graphic, position, animation, flags] : depth_level_render_elements) {
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

				std::string animation_name = Lua.get_component(animation->get_id())["current"];
				int frame = Lua.get_component(animation->get_id())["frame"];
				Sprite* sprite = get_frame(animation_name, frame);
				if (!sprite) {
					tdns_log.write("Trying to render, but sprite returned was invalid (nullptr).");
					tdns_log.write("Animation was: " + animation_name);
					tdns_log.write("Frame was: " + std::to_string(frame));
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

	void add_entity_to_scene(std::string scene, std::string entity) {
		auto& scene_manager = get_scene_manager();
		scene_manager.add_entity(scene, entity);
	}

	void draw_entity(int entity, Render_Flags flags) {
		EntityHandle handle;
		handle.id = entity;
		if (!handle) return;

		auto& render_engine = get_render_engine();
		render_engine.draw(handle, flags);
	}
	
	Sprite* get_frame(std::string animation, int frame) {
		if (animation.empty()) {
			tdns_log.write("Component asked for current frame but no active animation was set!");
			return (Sprite*)nullptr;
		}
		if (frame < 0) {
			std::string msg = "Frame index less than 0 for animation " + animation;
			tdns_log.write(msg);
			return (Sprite*)nullptr;
		}

		Animation* animation_p = asset_table.get_asset<Animation>(animation);
		if (!animation_p) {
			tdns_log.write("Could not find animation: " + animation);
			return (Sprite*)nullptr;
		}

		auto sprite_name = animation_p->get_frame(frame);
		auto frame_p = asset_table.get_asset<Sprite>(sprite_name);
		if (!frame_p) {
			tdns_log.write("Could not find sprite: " + sprite_name);
			return (Sprite*)nullptr;
		}

		return frame_p;
	}

	void register_animation(std::string name, std::vector<std::string> frames) {
		tdns_log.write("Registering animation: " + name, Log_Flags::File);
		
		Animation* animation = new Animation;
		for (auto& frame : frames) {
			animation->add_frame(frame);
		}

		asset_table.add_asset<Animation>(name, animation);

		auto a = asset_table.get_asset<Animation>(name);
		for (auto f : a->frames) {
			tdns_log.write(f);
		}
	}
	
	std::vector<std::string> get_frames(std::string animation_name) {
		Animation* animation = asset_table.get_asset<Animation>(animation_name);
		if (!animation) {
			tdns_log.write("Asked for animation's frames, but could not find that animation");
			tdns_log.write("Animation name: " + animation_name);
			return {};
		}
		
		return animation->frames;
	}

	void enable_input_channel(int channel) {
		auto& input_manager = get_input_manager();
		input_manager.enable_channel(channel);
	}
	
	void disable_input_channel(int channel) {
		auto& input_manager = get_input_manager();
		input_manager.disable_channel(channel);
	}

	bool is_key_down(GLFW_KEY_TYPE id, int mask) {
		auto& manager = get_input_manager();
		if (!(manager.mask & mask)) return false;
		return manager.is_down[id];
	}
	
	bool was_key_pressed(GLFW_KEY_TYPE id, int mask) {
		auto& manager = get_input_manager();
		return manager.was_pressed(id, mask);
	}

	bool was_chord_pressed(GLFW_KEY_TYPE mod_key, GLFW_KEY_TYPE cmd_key, int mask) {
		auto& manager = get_input_manager();
		return manager.chord(mod_key, cmd_key, mask);
	}

	void set_camera_offset(float x, float y) {
		auto& render_engine = get_render_engine();
		render_engine.camera_offset = { x, y };
	}
}
