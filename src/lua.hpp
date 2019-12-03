struct LuaState {
	lua_State* state;

	int init();
	int require(string script_name);
	void require_all();
	void test();
	int reload(string script_name);
} Lua;

void lua_error(int error_code);
int lua_print(lua_State* state);
void override_lua_stl_functions();
int prepend_to_lua_path(string directory);


