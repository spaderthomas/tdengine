struct Component;
struct LuaState {
	lua_State* raw_state;
	sol::state_view state;

	LuaState();
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

	std::string scripts;
	std::string actions;
	std::string animations;
	std::string battle;
	std::string components;
	std::string core;
	std::string cutscenes;
	std::string dialogue;
	std::string entities;
	std::string layouts;
	std::string libs;
	std::string prefabs;
	std::string saves;
	std::string scenes;
	std::string gstate; 
} Lua;

void init_lua();
