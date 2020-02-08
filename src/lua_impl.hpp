void LuaState::prepend_to_search_path(std::string directory) {
	directory = absolute_path(directory);
	directory += "/?.lua";
	normalize_path(directory);
	tdns_log.write("Adding directory to Lua include path: " + directory);
		
	// NEW_ITEM;old_path
	std::string old_path = Lua.state["package"]["path"];
	std::string new_path = directory + ";" + old_path;
	state["package"]["path"] = new_path;
}

int LuaState::init() {
	state.open_libraries(sol::lib::base, sol::lib::package, sol::lib::debug, sol::lib::string, sol::lib::math, sol::lib::table);
	
	prepend_to_search_path(absolute_path(path_join({"src", "scripts", "lua"})));

	return 0;
}

void LuaState::script_file(std::string relative_file_path) {
	auto result = state.script_file(script_path(relative_file_path), [](auto, auto pfr) {
		return pfr;
	});

	if (!result.valid()) {
		sol::error error = result;
		tdns_log.write(error.what());
	}
}

void LuaState::test() {
	sol::usertype<LuaEntity> entity_type = state.new_usertype<LuaEntity>("Entity");
	entity_type["id"] = &LuaEntity::id;
	//entity_type["name"] = &LuaEntity::name;
	//entity_type["update"] = &LuaEntity::update;
	//entity_type["add_component"] = &LuaEntity::add_component;
	//entity_type["get_id"] = &LuaEntity::get_id;

	script_file("tdengine.lua");

	std::shared_ptr<EntityManager> manager = std::make_shared<EntityManager>();
	auto entity = manager->create_entity("Spader");
}
