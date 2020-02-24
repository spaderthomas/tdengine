void LuaState::prepend_to_search_path(std::string directory) {
	directory = absolute_path(directory);
	directory += "/?.lua";
	normalize_path(directory);
	tdns_log.write("Adding directory to Lua include path: " + directory, Log_Flags::File);
		
	// NEW_ITEM;old_path
	std::string old_path = Lua.state["package"]["path"];
	std::string new_path = directory + ";" + old_path;
	state["package"]["path"] = new_path;
}

int LuaState::init() {
	state.open_libraries(sol::lib::base, sol::lib::package, sol::lib::debug, sol::lib::string, sol::lib::math, sol::lib::table);
	
	prepend_to_search_path(absolute_path(path_join({"src", "scripts", "lua"})));
	prepend_to_search_path(absolute_path(path_join({"src", "scripts", "lua", "libs"})));

	return 0;
}

void LuaState::script_dir(ScriptPath path) {
	for (auto it = directory_iterator(path.path); it != directory_iterator(); it++) {
		std::string next_path = it->path().string();
		normalize_path(next_path);
		// Make sure the path is a TDS file that has not been run
		if (is_regular_file(it->status())) {
			if (is_lua(next_path)) {
				script_file(ScriptPath(next_path));
			}
		}
		else if (is_directory(it->status())) {
			script_dir(next_path);
		}
	}
}

void LuaState::script_file(ScriptPath path) {
	tdns_log.write("Loaded script: " + path.path, Log_Flags::File);
	auto result = state.script_file(path.path, [](auto, auto pfr) {
		return pfr;
	});

	if (!result.valid()) {
		sol::error error = result;
		tdns_log.write("Failed to script file: " + path.path);
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
	component_type["get_id"] = &NewStuff::Component::get_id;
	component_type["get_entity"] = &NewStuff::Component::get_entity;

    state["tdapi"] = state.create_table();
	state["tdapi"]["draw_entity"] = &NewStuff::draw_entity;
	state["tdapi"]["register_animation"] = &NewStuff::register_animation;

	script_file(RelativePath("tdengine.lua"));
	script_dir(RelativePath("entities"));
	script_dir(RelativePath("components"));
}

void LuaState::update(float dt) {
	static std::shared_ptr<NewStuff::EntityManager> entity_manager = std::make_shared<NewStuff::EntityManager>();
	static std::shared_ptr<NewStuff::SceneManager> scene_manager = std::make_shared<NewStuff::SceneManager>();
	
	static auto spader = entity_manager->create_entity("Spader");
	static auto scene = scene_manager->create_scene("FirstScene");
	
	scene->add_entity(spader);
	scene->update(dt);

	draw_entity(spader);

	NewStuff::RenderEngine.render_for_frame();
}

sol::table LuaState::get_entity(int id) {
	return state["Entities"][id];
}
sol::table LuaState::get_component(int id) {
	return state["Components"][id];
}
