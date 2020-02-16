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
		std::string script = "local dt = ... \n" +  name + ":update(dt)";
		auto update_function = Lua.state.load(script);
		update_function(dt);
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
		std::string script = "local dt = ... \n" +  name + ":update(dt)";
		auto update_function = Lua.state.load(script);
		update_function(dt);
		
		for (auto& [kind, component] : components) {
			component->update(dt);
		}
	}

	void Entity::add_component(std::string name) {
		tdns_log.write("Called AC!" + name);
		components[name] = Component::create(name, id);
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
}
