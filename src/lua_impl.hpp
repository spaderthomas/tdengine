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
		tdns_log.write("Failed to script file: " + script_path(relative_file_path));
		tdns_log.write(error.what());
	}
}

void LuaState::test() {
	sol::usertype<NewStuff::Entity> entity_type = state.new_usertype<NewStuff::Entity>("Entity");
	entity_type["update"] = &NewStuff::Entity::update;
	entity_type["add_component"] = &NewStuff::Entity::add_component;
	entity_type["get_component"] = &NewStuff::Entity::get_component;
	entity_type["get_name"] = &NewStuff::Entity::get_name;
	entity_type["get_id"] = &NewStuff::Entity::get_id;

	sol::usertype<NewStuff::Component> component_type = state.new_usertype<NewStuff::Component>("Component");
	component_type["update"] = &NewStuff::Component::update;
	component_type["get_name"] = &NewStuff::Component::get_name;
	component_type["get_entity"] = &NewStuff::Component::get_entity;

	script_file("tdengine.lua");

	std::shared_ptr<NewStuff::EntityManager> manager = std::make_shared<NewStuff::EntityManager>();
	auto entity = manager->create_entity("Spader");
}
