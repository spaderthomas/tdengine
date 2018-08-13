struct {
	sol::state state;
	map<string, vector<string>> script_to_definitions;
	map<string, string> definitions_to_script;

	// Some scripts are things we don't wanna be able to select in ImGui, so just load those in separately
	vector<string> scripts = {
		"props",
		"boon",
		"npc"
	};
	vector<string> imgui_ignored_scripts = {
		"utils",
		"tiles",
		"intro",
		"state"
	};

	void init() {
		state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io);
		
		// Register C++ functions in Lua
		state.set_function("show_text", &Text_Box::begin, &game_layer.text_box);
		state.set_function("set_state", &State_Component::set_state);


		// Load scripts
		auto error_handler = [](lua_State*, sol::protected_function_result pfr) {
			sol::error err = pfr;
			string msg = string("Error in script: ") + err.what();
			tdns_log.write(msg);
			return pfr;
		};

		// Note: this has to go first so utils gets loaded. @hack?
		for (auto& script : imgui_ignored_scripts) {
			string path = "..\\..\\src\\scripts\\" + script + ".lua";
			state.safe_script_file(path, error_handler);

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

		for (auto& script : scripts) {
			string path = "..\\..\\src\\scripts\\" + script + ".lua";
			state.safe_script_file(path, error_handler);

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

	sol::table get_npc_properties(string name) {
		sol::table character = state["npc"][name];
		return character;
	}
} Lua;
