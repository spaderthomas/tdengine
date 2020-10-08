void LuaState::prepend_to_search_path(std::string directory) {
	directory = absolute_path(directory);
	directory += "/?.lua";
	normalize_path(directory);
	tdns_log.write("Adding directory to Lua include path: " + directory, Log_Flags::File);
		
	// NEW_ITEM;old_path
	std::string old_path = Lua.state["package"]["path"];
	std::string new_path = directory + ";" + old_path;
	state["package"]["path"] = new_path;
}

int LuaState::init() {
	state.open_libraries(
        sol::lib::base,
		sol::lib::package,
		sol::lib::debug,
		sol::lib::string,
		sol::lib::math,
		sol::lib::table,
		sol::lib::io);
	
	prepend_to_search_path(absolute_path(path_join({"src", "scripts", "libs"})));
	prepend_to_search_path(absolute_path(path_join({"src", "scripts", "core"})));
	prepend_to_search_path(absolute_path(path_join({"src", "scripts"})));

	LoadImguiBindings();

	sol::usertype<Entity> entity_type = state.new_usertype<Entity>("Entity");
	entity_type["update"] = &Entity::update;
	entity_type["add_component"] = &Entity::add_component;
	entity_type["get_component"] = &Entity::get_component;
	entity_type["all_components"] = &Entity::all_components;
	entity_type["get_name"] = &Entity::get_name;
	entity_type["get_id"] = &Entity::get_id;

	sol::usertype<Component> component_type = state.new_usertype<Component>("Component");
	component_type["update"] = &Component::update;
	component_type["get_name"] = &Component::get_name;
	component_type["get_id"] = &Component::get_id;
	component_type["get_entity"] = &Component::get_entity;

	register_lua_api();
	
	script_dir(RelativePath("libs"));
	script_dir(RelativePath("core"));
	script_dir(RelativePath("entities"));
	script_dir(RelativePath("components"));
	script_dir(RelativePath("scenes"));

	return 0;
}

void LuaState::script_dir(ScriptPath path) {
	for (auto it = directory_iterator(path.path); it != directory_iterator(); it++) {
		std::string next_path = it->path().string();
		normalize_path(next_path);
		// Make sure the path is a TDS file that has not been run
		if (is_regular_file(it->status())) {
			if (is_lua(next_path)) {
				script_file(ScriptPath(next_path));
			}
		}
		else if (is_directory(it->status())) {
			script_dir(next_path);
		}
	}
}

void LuaState::script_file(ScriptPath path) {
	tdns_log.write("Loaded script: " + path.path, Log_Flags::File);
	auto result = state.script_file(path.path, [](auto, auto pfr) {
		return pfr;
	});

	if (!result.valid()) {
		sol::error error = result;
		tdns_log.write("Failed to script file: " + path.path);
		tdns_log.write(error.what());
	}

	file_watcher.watch(path, [this, path](){
		tdns_log.write("Reloaded Lua script: " + path.path);
		this->script_file(path);
	});
}

void LuaState::test() {
}

void LuaState::update(float dt) {
	
}

sol::table LuaState::get_entity(int id) {
	return state["Entities"][id];
}
sol::table LuaState::get_component(int id) {
	return state["Components"][id];
}
sol::table LuaState::get_component(Component* component) {
	return state["Components"][component->get_id()];
}
sol::table LuaState::get_component(int entity, std::string which) {
	auto& entity_manager = get_entity_manager();
	auto entity_ptr = entity_manager.get_entity(entity);

	if (!entity_ptr) {
		std::string message = "Tried to get " + which + "component for entity " + std::to_string(entity) + ", but entity did not exist.";
		tdns_log.write(message);
		return sol::table();
	}

	auto component_ptr = entity_ptr->get_component(which);
	return get_component(component_ptr);
}
