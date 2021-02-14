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

ComponentManager& get_component_manager() {
	static ComponentManager manager;
	return manager;
}

Component* ComponentManager::create_component(std::string name, int entity, sol::table data) {
	auto component = std::make_unique<Component>();
	auto id = Component::next_id++;
	component->id = id;
	component->name = name;
	component->entity = entity;

	auto& c = *(component.get());
	sol::protected_function on_component_created = Lua.state["on_component_created"];
	auto result = on_component_created(c, data);
	if (!result.valid()) {
		sol::error error = result;
		tdns_log.write("Failed to create component. Entity ID: " + std::to_string(entity) + ". Component name: " + name + ". Lua error:");
		tdns_log.write(error.what());
	}

	
	components[component->id] = std::move(component);
	return components[id].get();
}

Component* ComponentManager::get_component(int id) {
	for (auto& [other_id, component] : components) {
		if (id == other_id) return component.get();
	}

	return nullptr;
}
	
bool ComponentManager::has_component(int id) {
	return components.find(id) != components.end();
}

void ComponentManager::destroy_component(int id) {
	bool found = components.find(id) != components.end();
	if (!found) return;
	
	auto component = components[id].get();
	
	sol::protected_function on_component_created = Lua.state["on_component_destroyed"];
	auto result = on_component_created(component);
	if (!result.valid()) {
		sol::error error = result;
		tdns_log.write("Failed to destroy component. ID was: " + std::to_string(id));
		tdns_log.write(error.what());
	}

	components.erase(id);
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

std::string Entity::debug_string() {
	return get_name() + "(" + std::to_string(get_id()) + ")";
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

Component* Entity::add_component(std::string name, sol::table data) {
	// Don't double add components
	if (components.find(name) != components.end()) {
		return components[name];
	}

	auto& component_manager = get_component_manager();
	auto component = component_manager.create_component(name, id, data);
	
	if (component) {
		components[name] = component;
	    return component;
	} else {
		// For example, a typo in the component name, or if its init() is broken
		return nullptr;
	}
}

void Entity::remove_component(std::string name) {
	if (!has_component(name)) return;
	tdns_log.write("remove_component() is unimplemented");
}

bool Entity::has_component(std::string name)
{
	return components.find(name) != components.end();
}

Component* Entity::get_component(std::string name) {
	if (!has_component(name)) return nullptr;
	return components[name];
}

std::vector<Component*> Entity::all_components() {
	std::vector<Component*> out;
	for (auto& [name, component] : components) {
		out.push_back(component);
	}
	return out;
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

int EntityManager::create_entity(std::string name) {
	// Construct the entity
	auto inserted = entities.emplace(Entity::next_id, std::make_unique<Entity>(name, Entity::next_id));
	Entity::next_id++;
	auto it = inserted.first;
	Entity& entity = *it->second;

	// Add it to Lua
	sol::protected_function on_entity_created = Lua.state["on_entity_created"];
	auto result = on_entity_created(entity);

	// This one is hit if there's an error in the Lua code for the entity		
	if (!result.valid()) {
		sol::error error = result;
		std::string message = "You tried to create an entity named " + name;
		message += ", but it errored out in init(). Here's the Lua error:";
		tdns_log.write(message);
		tdns_log.write(error.what());
	}
	// Otherwise, check the return code. We'll return 1 if we couldn't find the entity type.
	else {
		int code = result;

		if (code) {
			std::string message = "You tried to create an entity named " + name;
			message += ", but there is no Lua class for that entity";
			tdns_log.write(message);

			// Delete the C++ object we just created
			entities.erase(entities.find(entity.id));
			return -1;
		}
	}

	return entity.id;
}

void EntityManager::destroy_entity(int id) {
	bool found = entities.find(id) != entities.end();
	if (!found) return;
	
	auto entity = entities[id].get();

	auto& component_manager = get_component_manager();
	for (auto [name, component] : entity->components) {
		component_manager.destroy_component(component->get_id());
	}
	
	sol::protected_function on_entity_created = Lua.state["on_entity_destroyed"];
	auto result = on_entity_created(entity);
	if (!result.valid()) {
		sol::error error = result;
		tdns_log.write("Failed to destroy entity. ID was: " + std::to_string(id));
		tdns_log.write(error.what());
	}

	entities.erase(id);
}

CutsceneManager& get_cutscene_manager() {
	static CutsceneManager manager;
	return manager;
}

void CutsceneManager::update(float dt) {
	sol::protected_function update_cutscene = Lua.state["tdengine"]["update_cutscene"];
	auto result = update_cutscene(dt);
	if (!result.valid()) {
		sol::error error = result;
		tdns_log.write("Failed to update cutscene."); // @log
		tdns_log.write(error.what());
	}
}
