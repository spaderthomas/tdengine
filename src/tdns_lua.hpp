struct {
	sol::state state;

	void init() {
		state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io);
		load_scripts();
	}

	void load_scripts() {
		vector<string> scripts = {
			"../../src/scripts/static_background.lua",
			"../../src/scripts/static_foreground.lua",
		};
		auto error_handler = [](lua_State*, sol::protected_function_result pfr) {
			sol::error err = pfr;
			string msg = string("Error in script: ") + err.what();
			tdns_log.write(msg);
			return pfr;
		};

		for (auto& script : scripts) {
			state.safe_script_file(script, error_handler);
		}
	}
} Lua;