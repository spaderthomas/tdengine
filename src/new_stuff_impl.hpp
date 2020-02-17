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
}

namespace API {
	void draw_entity(EntityHandle entity, Render_Flags flags) {
		if (!entity) return;
		
		auto graphic = entity->get_component("GraphicComponent");
		auto position = entity->get_component("PositionComponent");
		if (!graphic || !position) {
			tdns_log.write("Called draw_entity(), but entity was lacking component.");
			tdns_log.write("Entity name: " + entity->get_name());
			tdns_log.write("Entity ID: " + entity->get_id());
			return;
		}

		renderer.draw(graphic, position, entity, flags);
	}
}
