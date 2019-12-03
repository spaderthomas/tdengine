void lua_error(int error_code) {
  if(!error_code) return;

  // If we've got something on the top of the stack, it's an error message
  if(!lua_isnil(Lua.state, -1)) {
	  auto message = string(lua_tostring(Lua.state, -1));
	  tdns_log.write("Lua error! Error code: " + to_string(error_code) + ". Error message: " + message);
	  lua_pop(Lua.state, 1);
  } else {
	  tdns_log.write("Lua error! Error code: " + to_string(error_code) + ". But there was no message...");
  }
}

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

int prepend_to_lua_path(string directory) {
	directory = absolute_path(directory);
	directory += "/?.lua";
	normalize_path(directory);
	tdns_log.write("Adding directory to Lua include path: " + directory);
		
	// get field "path" from table at top of stack (-1)
	lua_getglobal(Lua.state, "package");
	lua_getfield(Lua.state, -1, "path");
	
	// NEW_ITEM;old_path
	std::string new_path = directory + ";" + lua_tostring(Lua.state, -1);
	
	lua_pop(Lua.state, 1);
	lua_pushstring(Lua.state, new_path.c_str());
	lua_setfield(Lua.state, -2, "path"); // set the field "path" in table at -2 with value at top of stack
	lua_pop(Lua.state, 1); // get rid of package table from top of stack
	
	return 0;
}
   
int LuaState::init() {
	Lua.state = luaL_newstate();
	if (!Lua.state) {
		tdns_log.write("Failed to initialize Lua state.");
		return 1;
	}

	luaL_openlibs(Lua.state);
	override_lua_stl_functions();
	prepend_to_lua_path(absolute_path(path_join({"src", "scripts", "lua"})));
	prepend_to_lua_path(absolute_path(path_join({"src", "scripts", "lua", "second"})));

	require_all();
	
	return 0;
}

int LuaState::require(string script_name) {
	lua_getglobal(state, "require");
	lua_pushstring(state, script_name.c_str());

	int error_code = lua_pcall(state, 1, 1, 0);
	if (!error_code) {
		lua_pop(state, 1);
		lua_error(error_code);
	}

	return error_code;
}

void LuaState::require_all() {
	require("second");
	require("first");
}

int LuaState::reload(string script_name) {
	lua_getglobal(Lua.state, "first");
	lua_getfield(Lua.state, -1, "some_table");
	lua_pushstring(Lua.state, "package");

	return 0;
}

void LuaState::test() {
	luaL_dostring(state, "print(\"I did a string!\")");
	luaL_dostring(state, "first.first()");
}
