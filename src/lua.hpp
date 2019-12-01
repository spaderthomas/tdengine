struct LuaState {
	lua_State* state;
} Lua;

int lua_print(lua_State* state) {
	int count_args = lua_gettop(state);
	if(!count_args) return 0;

	std::string message;
	
	lua_getglobal(state, "tostring");
	for(int arg_index = 1; arg_index <= count_args; arg_index++) {
		const char *s;
		lua_pushvalue(state, -1);  /* function to be called */
		lua_pushvalue(state, arg_index);   /* value to print */
		lua_call(state, 1, 1);
		s = lua_tostring(state, -1);  /* get result */
		
		if(!s) return luaL_error(state, LUA_QL("tostring") " must return a string to " LUA_QL("print"));
		
		if(arg_index > 1) message += "\t";

		message += s;
		
		lua_pop(state, 1);  /* pop result */
  }
	
	tdns_log.write(message);
	return 0;
}

void override_lua_stl_functions() {
	lua_register(Lua.state, "print", &lua_print);
}

int init_lua() {
	Lua.state = luaL_newstate();
	if (!Lua.state) {
		tdns_log.write("Failed to initialize Lua state.");
		return 1;
	}

	luaL_openlibs(Lua.state);
	override_lua_stl_functions();

	auto test_file = absolute_path(path_join({"src", "scripts", "test.lua"}));
	normalize_path(test_file);
	tdns_log.write(test_file);
	luaL_dofile(Lua.state, test_file.c_str());
	return 0;
}
   
