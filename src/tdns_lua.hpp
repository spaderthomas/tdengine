struct LuaState {
	sol::state state;
	map<string, vector<string>> script_to_definitions;
	map<string, string> definitions_to_script;

	// Some scripts are things we don't wanna be able to select in ImGui, so just load those in separately
	vector<string> imgui_shown_scripts;
	vector<string> imgui_ignored_scripts;

	void init() {
		state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io, sol::lib::package, sol::lib::math, sol::lib::table);

		// Register functions and classes
		state.set_function("are_colliding", &are_entities_colliding);
		state.set_function("entity_name", &entity_name);
		state.set_function("entity_id", &entity_id);
		state.set_function("on_collide", &on_collide);

		state.new_usertype<EntityHandle>(
			"EntityHandle"
		);	

		state.new_usertype<Entity>(
			"Entity",
			"get_component", &Entity::get_component,
			"lua_id", &Entity::lua_id
		);
		
		state.new_usertype<Level>(
			"Level",
			"name", &Level::name,
			"load", &Level::load,
			"entities", &Level::entity_handles,
			"count_entities", &Level::count_entities
		);

		// Load scripts
		auto error_handler = [](lua_State*, sol::protected_function_result pfr) {
			sol::error err = pfr;
			string msg = string("Error in script: ") + err.what();
			tdns_log.write(msg);
			return pfr;
		};
		state.safe_script_file(absolute_path("src\\scripts\\meta.lua"), error_handler);
		sol::table files = state["scripts"];
		for (auto& kvp : files) {
			sol::table file = kvp.second;
			string script = file["name"];
			string path = absolute_path("src\\scripts\\") + script + ".lua";

			file["imgui_ignore"] ? 
				imgui_ignored_scripts.push_back(script) :
				imgui_shown_scripts.push_back(script);

			state.safe_script_file(path, error_handler);
			if (file["check_for_defs"]) {
				vector<string> defined;
				sol::optional<sol::table> entities = state[script];
				if (entities) {
					for (auto& entity : entities.value()) {
						string name = entity.first.as<string>();
						defined.push_back(name);
						definitions_to_script[name] = script;
					}
				}

				sort(defined.begin(), defined.end());
				script_to_definitions[script] = defined;
			}
		}
	}
};
LuaState Lua;
