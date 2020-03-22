namespace NewStuff {
	struct Component;
}
struct LuaState {
	sol::state state;

	int init();
	int require(std::string script_name);
	void require_all();
	int reload(std::string script_name);
	void prepend_to_search_path(std::string directory);
	void script_dir(ScriptPath path);
	void script_file(ScriptPath path);

	// purely for testing stuff
	void test();
	void update(float dt);

	sol::table get_entity(int id);
	sol::table get_component(int id);
	sol::table get_component(NewStuff::Component* component);
} Lua;


