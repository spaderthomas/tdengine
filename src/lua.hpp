struct Component;
struct LuaState {
	lua_State* raw_state;
	sol::state_view state;

	LuaState();
	int init();
	int require(std::string script_name);
	void require_all();
	int reload(std::string script_name);
	void prepend_to_search_path(std::string directory);
	void script_dir(ScriptPath path);
	void script_file(ScriptPath path);

	sol::table get_entity(int id);
	sol::table get_component(int id);
	sol::table get_component(Component* component);
	sol::table get_component(int entity, std::string which);
} Lua;

