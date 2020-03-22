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
	
	prepend_to_search_path(absolute_path(path_join({"src", "scripts"})));
	prepend_to_search_path(absolute_path(path_join({"src", "scripts", "libs"})));
	prepend_to_search_path(absolute_path(path_join({"src", "scripts", "core"})));

	LoadImguiBindings();

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

    state["tdengine"] = state.create_table();
	state["tdengine"]["add_entity_to_scene"] = &NewStuff::add_entity_to_scene;
	state["tdengine"]["register_animation"] = &NewStuff::register_animation;
	state["tdengine"]["get_frames"] = &NewStuff::get_frames;
	state["tdengine"]["enable_input_channel"] = &NewStuff::enable_input_channel;
	state["tdengine"]["disable_input_channel"] = &NewStuff::disable_input_channel;
	state["tdengine"]["is_key_down"] = &NewStuff::is_key_down;
	state["tdengine"]["was_key_pressed"] = &NewStuff::was_key_pressed;
	state["tdengine"]["was_chord_pressed"] = &NewStuff::was_chord_pressed;
	state["tdengine"]["set_camera_offset"] = &NewStuff::set_camera_offset;

    state["tdengine"]["internal"] = state.create_table();
	state["tdengine"]["internal"]["draw_entity"] = &NewStuff::draw_entity;

	state["tdengine"]["InputChannel"] = state.create_table();
	state["tdengine"]["InputChannel"]["None"] = INPUT_MASK_NONE;
	state["tdengine"]["InputChannel"]["ImGui"] = INPUT_MASK_IMGUI;
	state["tdengine"]["InputChannel"]["Editor"] = INPUT_MASK_EDITOR;
	state["tdengine"]["InputChannel"]["Game"] = INPUT_MASK_GAME;
	state["tdengine"]["InputChannel"]["All"] = INPUT_MASK_ALL;

	script_dir(RelativePath("libs"));
	script_dir(RelativePath("core"));
	script_dir(RelativePath("entities"));
	script_dir(RelativePath("components"));
	script_dir(RelativePath("scenes"));

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

	file_watcher.watch(path, [this, path](){
		tdns_log.write("Reloaded Lua script: " + path.path);
		this->script_file(path);
	});
}

void LuaState::test() {
	auto& scene_manager = NewStuff::get_scene_manager();
	scene_manager.create_scene("Overworld");

	auto& entity_manager = NewStuff::get_entity_manager();
	entity_manager.create_entity("Editor");
}

void LuaState::update(float dt) {
	auto& render_engine = NewStuff::get_render_engine();
	auto& entity_manager = NewStuff::get_entity_manager();
	
	//draw_entity(spader);
	entity_manager.update(dt);
	render_engine.render();
}

sol::table LuaState::get_entity(int id) {
	return state["Entities"][id];
}
sol::table LuaState::get_component(int id) {
	return state["Components"][id];
}
sol::table LuaState::get_component(NewStuff::Component* component) {
	return state["Components"][component->get_id()];
}
