LuaState::LuaState() :
	raw_state(luaL_newstate()),
	state(raw_state)
{}

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

void init_lua() {
	auto& lua_manager = Lua;
	
	lua_manager.state.open_libraries(
        sol::lib::base,
        sol::lib::bit32,
		sol::lib::debug,
		sol::lib::io,
		sol::lib::jit,
		sol::lib::math,
		sol::lib::os,
		sol::lib::package,
		sol::lib::string,
		sol::lib::table);

	// Set up paths
	lua_manager.scripts = absolute_path(path_join({"src", "scripts"}));
	lua_manager.actions = path_join({lua_manager.scripts, "actions"});
	lua_manager.animations = path_join({lua_manager.scripts, "animations"});
	lua_manager.battle = path_join({lua_manager.scripts, "battle"});
	lua_manager.components = path_join({lua_manager.scripts, "components"});
	lua_manager.core = path_join({lua_manager.scripts, "core"});
	lua_manager.cutscenes = path_join({lua_manager.scripts, "cutscenes"});
	lua_manager.dialogue = path_join({lua_manager.scripts, "dialogue"});
	lua_manager.entities = path_join({lua_manager.scripts, "entities"});
	lua_manager.layouts = path_join({lua_manager.scripts, "layouts"});
	lua_manager.libs = path_join({lua_manager.scripts, "libs"});
	lua_manager.prefabs = path_join({lua_manager.scripts, "prefabs"});
	lua_manager.saves = path_join({lua_manager.scripts, "saves"});
	lua_manager.scenes = path_join({lua_manager.scripts, "scenes"});
	lua_manager.gstate = path_join({lua_manager.scripts, "state"}); 

	// Give those paths to Lua
	lua_manager.prepend_to_search_path(lua_manager.scripts);
	lua_manager.prepend_to_search_path(lua_manager.libs);
	lua_manager.prepend_to_search_path(lua_manager.core);
	lua_manager.prepend_to_search_path(lua_manager.animations);
	lua_manager.prepend_to_search_path(lua_manager.saves);

	// Bind all C functions
	LoadImguiBindings();
	register_lua_api();
	
	lua_manager.script_dir(RelativePath("libs"));
	lua_manager.script_dir(RelativePath("core"));

	lua_manager.state.script("tdengine.initialize()");
	
	lua_manager.script_dir(RelativePath("entities"));
	lua_manager.script_dir(RelativePath("components"));
	lua_manager.script_dir(RelativePath("scenes"));
	lua_manager.script_dir(RelativePath("actions"));

	lua_manager.state.script("tdengine.load_editor()");
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

sol::table LuaState::get_entity(int id) {
	return state["tdengine"]["entities"][id];
}
sol::table LuaState::get_component(int id) {
	return state["tdengine"]["components"][id];
}
sol::table LuaState::get_component(Component* component) {
	return state["tdengine"]["components"][component->get_id()];
}
sol::table LuaState::get_component(int entity, std::string which) {
	auto& entity_manager = get_entity_manager();
	auto entity_ptr = entity_manager.get_entity(entity);

	if (!entity_ptr) {
		std::string message = "Tried to get " + which + " for entity " + std::to_string(entity) + ", but entity did not exist.";
		tdns_log.write(message);
		return sol::table();
	}

	auto component_ptr = entity_ptr->get_component(which);
	return get_component(component_ptr);
}
