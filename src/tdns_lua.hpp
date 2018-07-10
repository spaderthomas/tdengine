
struct {
	sol::state state;
	map<string, vector<string>> script_to_definitions;
	map<string, string> definitions_to_script;
	vector<string> scripts = {
		"props",
		"boon",
	};
	vector<string> imgui_ignored_scripts = {
		"utils",
		"tiles"
	};

	void init() {
		state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io);
		load_scripts();
	}

	void load_scripts() {
		auto error_handler = [](lua_State*, sol::protected_function_result pfr) {
			sol::error err = pfr;
			string msg = string("Error in script: ") + err.what();
			tdns_log.write(msg);
			return pfr;
		};

		// Some scripts are things we don't wanna be able to select in ImGui
		// So just load those in separately
		// Also note: this has to go first so utils gets loaded. @hack?
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
		// Each script has a single table with its filename as the name containing its definitions
		// Pull out the definitions in the file and put them in a vector that the filename maps to
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

	
} Lua;