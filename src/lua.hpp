struct LuaState {
	sol::state state;

	int init();
	int require(string script_name);
	void require_all();
	void test();
	int reload(string script_name);
	void prepend_to_search_path(string directory);
	void script_file(string relative_file_path);
} Lua;


