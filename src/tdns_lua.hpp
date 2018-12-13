struct LuaState {
	sol::state state;

	void recursive_add_dir(string dir) {
		// Always check for the directory's init file first
		string maybe_init_file = dir + "\\init.lua";
		ifstream f(maybe_init_file.c_str());
		if (f.good()) {
			state.script_file(maybe_init_file);
		}

		for (auto it = directory_iterator(dir); it != directory_iterator(); it++) {
			string path = it->path().string();
			if (is_regular_file(it->status())) {
				if (is_lua(path)) {
					// Don't double run the init file 
					if (!path.compare(maybe_init_file)) {
						continue;
					}
					state.script_file(path);
				}
			}
			else if (is_directory(it->status())) {
				recursive_add_dir(path);
			}

		}
	}

	void init() {
		state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io, sol::lib::package, sol::lib::math, sol::lib::table);
		bind_functions();

		// Userdata
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
			"draw", &Level::draw,
			"entities", &Level::entities
		);

		
		// Load scripts
		state.script_file(absolute_path("src\\scripts\\meta.lua"));
		state.script_file(absolute_path("src\\scripts\\init.lua"));
		sol::table files = state["meta"]["scripts"];

		// Load everything that needs to be in the global namespace from scripts
		for (auto& kvp : files) {
			sol::table file = kvp.second;
			string script = file["name"];
			if (script == "init.lua") continue;

			if (file["is_dir"]) {
				recursive_add_dir(absolute_path("src\\scripts\\") + script);
				continue;
			}

			string path = absolute_path("src\\scripts\\") + script + ".lua";
			state.script_file(path);
		}
	}

	void init_after_load() {
		sol::table after_load = state["meta"]["after_load"];
		for (auto& kvp : after_load) {
			sol::function func = kvp.second;
			func();
		}
	}

	void run_game_update(float dt) {
		//@hack either a better way to find the function with middleclass or dont use middleclass
		sol::function lua_update = state["Game"]["__declaredMethods"]["update"]; 
		lua_update(state["game"], dt);
	}
	sol::table entity_table() {
		return state["entity"];
	}
};
LuaState Lua;
