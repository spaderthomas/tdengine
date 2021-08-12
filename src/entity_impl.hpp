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

Component* ComponentManager::create_component(std::string name, int entity) {
	auto component = std::make_unique<Component>();
	auto id = Component::next_id++;
	component->id = id;
	component->name = name;
	component->entity = entity;
	
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
	return std::to_string(get_id()) + ", " + name;
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

	auto& component_manager = get_component_manager();
	auto component = component_manager.create_component(name, id);
	
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

Entity* EntityManager::get_entity(int id) {
	auto it = entities.find(id);
	if (it == entities.end()) return nullptr;

	return it->second.get();
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

	tdns_log.write("@entity_create: " + std::to_string(entity.id) + ", " + name, Log_Flags::File);

	return entity.id;
}

void EntityManager::destroy_entity(int id) {
	auto entity = get_entity(id);
	if (!entity) return;

	auto& component_manager = get_component_manager();
	for (auto [name, component] : entity->components) {
		component_manager.destroy_component(component->get_id());
	}

	entities.erase(id);
}

void EntityManager::destroy_flagged_entities() {
	for (auto entity : entities_to_destroy) {
		auto& render_engine = get_render_engine();
		render_engine.remove_entity(entity);
		
		auto& physics_engine = get_physics_engine();
		physics_engine.remove_entity(entity);
		
		auto& interaction_system = get_interaction_system();
		interaction_system.remove_entity(entity);

		destroy_entity(entity);
	}

	entities_to_destroy.clear();
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

UpdateSystem& get_update_system() {
	static UpdateSystem system;
	return system;
}

void UpdateSystem::run_collision_callbacks(float dt) {
	auto& physics_engine = get_physics_engine();
	sol::protected_function on_collision = Lua.state["on_collision"];

	auto run_collision_callback = [&](int entity_to_run_callback, int arg) {
		auto result = on_collision(entity_to_run_callback, arg);
		if (!result.valid()) {
			sol::error error = result;
			tdns_log.write("on_collision failed for " + std::to_string(entity_to_run_callback) + "Lua error: ");
			tdns_log.write(error.what());
		}
	};
	
	for (const auto& collision : physics_engine.collisions) {
		run_collision_callback(collision.entity, collision.other);
		run_collision_callback(collision.other,  collision.entity);
	}
}

void UpdateSystem::run_interaction_callback(float dt) {
	// Read for updates needed from interaction system
	auto& interaction_system = get_interaction_system();
	if (interaction_system.interacted_with >= 0) {
		sol::protected_function on_interaction = Lua.state["on_interaction"];
		auto result = on_interaction(interaction_system.interacted_with);
		if (!result.valid()) {
			sol::error error = result;
			tdns_log.write("@on_interaction_failure, " + std::to_string(interaction_system.interacted_with));
			tdns_log.write("Lua error:");
			tdns_log.write(error.what());
		}		
	}
}

void UpdateSystem::run_entity_updates(float dt) {
	auto& entity_manager = get_entity_manager();
	for (auto& [id, entity] : entity_manager.entities) {
		entity->update(dt);
	}
}

void UpdateSystem::do_paused_update(float dt) {
	auto& entity_manager = get_entity_manager();
	for (auto& [id, entity] : entity_manager.entities) {
		if (entity->get_name() == "Editor") {
			entity->update(dt);
		}
		
		auto graphic = entity->get_component("Graphic");
		if (graphic) graphic->update(dt);
	}
}
