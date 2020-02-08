LuaComponent LuaComponent::create(string name, int entity) {
	LuaComponent component;
	component.name = name;
	component.entity = entity;
	string script = name + ":new()";
	Lua.state.script(script);
	return component;
}

void LuaComponent::update(float dt) {
	string script = "local dt = ... \n" +  name + ":update(dt)";
	auto update_function = Lua.state.load(script);
	update_function(dt);
}

struct SpaderComponent : LuaComponent {
	int coolness = 400;
	int fun = 1; 
};

LuaEntity::LuaEntity(string name, int id) {
	this->name = name;
	this->id = id;
}

int LuaEntity::get_id() {
	return id;
}

void LuaEntity::update(float dt) {
	string script = "local dt = ... \n" +  name + ":update(dt)";
	auto update_function = Lua.state.load(script);
	update_function(dt);
		
	for (auto& [kind, component] : components) {
		component.update(dt);
	}
}

void LuaEntity::add_component(string name) {
	components[name] = LuaComponent::create(name, id);
}


LuaEntity* EntityManager::get_entity(int id) {
	return entities[id].get();
}

bool EntityManager::has_entity(int id) {
	return entities.find(id) != entities.end();
}

LuaEntityHandle EntityManager::create_entity(string name) {
	// Construct the entity
	auto inserted = entities.emplace(LuaEntity::next_id, make_unique<LuaEntity>(name, LuaEntity::next_id));
	auto it = inserted.first;
	LuaEntity& entity = *it->second;

	// Add it to Lua
	sol::protected_function create_handle = Lua.state["create_handle"];
	auto result = create_handle(entity);
	if (!result.valid()) {
		sol::error error = result;
		tdns_log.write(error.what());
	}
	
	// Return a handle
	LuaEntityHandle handle;
	handle.id = entity.id;
	handle.manager = this;
	return handle;
}

LuaEntity* LuaEntityHandle::operator->() const {
	return get();
}

LuaEntity* LuaEntityHandle::get() const {
	fox_assert(manager->has_entity(id));
	return manager->get_entity(id);
}
