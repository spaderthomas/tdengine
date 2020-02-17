struct LuaState {
	sol::state state;

	int init();
	int require(std::string script_name);
	void require_all();
	void test();
	int reload(std::string script_name);
	void prepend_to_search_path(std::string directory);
	void script_dir(ScriptPath path);
	void script_file(ScriptPath path);
} Lua;


