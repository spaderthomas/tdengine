int API::alloc_entity(std::string name) {
	auto& entity_manager = get_entity_manager();
	return entity_manager.create_entity(name);
}

void API::free_entity(int entity) {
	auto& entity_manager = get_entity_manager();
	auto& physics_engine = get_physics_engine();
	auto& render_engine = get_render_engine();
	auto& interaction_system = get_interaction_system();
	
	render_engine.remove_entity(entity);
	physics_engine.remove_entity(entity);
	interaction_system.remove_entity(entity);
	
	entity_manager.entities_to_destroy.push_back(entity);
}

int API::alloc_component(int entity, const char* name) {
	auto& manager = get_component_manager();
	auto component = manager.create_component(std::string(name), entity);
	return component->get_id();
}

void API::free_component(int component) {
	auto& manager = get_component_manager();
	return manager.destroy_component(component);
}

std::string API::component_name(int id) {
	auto& manager = get_component_manager();
	auto component = manager.get_component(id);
	if (!component) {
		tdns_log.write("component_name(): no such component: " + std::to_string(id));
		return nullptr;
	}
	return component->get_name();
}

std::string API::entity_name(int id) {
	auto& entity_manager = get_entity_manager();
	auto entity = entity_manager.get_entity(id);
	if (!entity) {
		tdns_log.write("@entity_name: no such entity: " + std::to_string(id));
		return "@UNKNOWN_NAME_PLEASE_FIX_ME";
	}
	return entity->get_name();
}

bool API::has_component(int id, const char* name) {
	auto& entity_manager = get_entity_manager();
	auto entity = entity_manager.get_entity(id);
	if (!entity) {
		tdns_log.write("has_component(): no such entity: " + std::to_string(id));
		return false;
	}
	return entity->has_component(name);
}

int API::get_component(int id, const char* name) {
	auto& entity_manager = get_entity_manager();
	auto entity = entity_manager.get_entity(id);
	if (!entity) {
		tdns_log.write("get_component(): no such entity: " + std::to_string(id));
		return -1;
	}

	auto component = entity->get_component(name);
	if (!component) {
		std::string message = "get_component(): no such component. ";
		message += name;
		message += "(" + std::to_string(id) + ")";
		tdns_log.write(message);
		return -1;
	}

	return component->get_id();
}

int API::add_component(int id, const char* name) {
	auto& entity_manager = get_entity_manager();
	auto entity = entity_manager.get_entity(id);
	if (!entity) {
		tdns_log.write("add_component(): no such entity: " + std::to_string(id));
		return -1;
	}

	auto component = entity->add_component(name);
	return component ? component->get_id() : -1;
}

sol::object API::all_components(int id) {
	auto& entity_manager = get_entity_manager();
	auto entity = entity_manager.get_entity(id);
	if (!entity) {
		tdns_log.write("all_components(): no such entity: " + std::to_string(id));
		return sol::make_object(Lua.state, sol::lua_nil);
	}

	auto out = Lua.state.create_table();
	auto components = entity->all_components();
	for (int i = 0; i < components.size(); i++) {
		auto component = components[i];
		out[i + 1] = component->id;
	}
	
	return out;
}

void API::register_player(int entity) {
	auto& entity_manager = get_entity_manager();
	entity_manager.player = entity;
	
	auto vision = Lua.get_component(entity, "PlayerVision");
	if (!vision) {
		tdns_log.write("@no_vision_for_player");
	}

	Collider collider;
	collider.entity = entity;
	collider.extents.x = vision["extents"]["x"];
	collider.extents.y = vision["extents"]["y"];
	collider.offset.x = vision["offset"]["x"];
	collider.offset.y = vision["offset"]["y"];

	auto& interaction = get_interaction_system();
	interaction.player_vision = collider;
}

void API::draw_entity(int id) {
	auto& entity_manager = get_entity_manager();
	auto entity = entity_manager.get_entity(id);
	
	// Grab the relevant components
	auto check_component_exists = [entity](auto component, std::string component_type) -> bool {
		if (component) return true;

		std::string message = "@draw_missing_component: " + entity->debug_string();
		message += ", " + component_type;
		tdns_log.write(message);
		return false;
	};
	
	auto graphic_ptr = entity->get_component("Graphic");
	if (!check_component_exists(graphic_ptr, "Graphic")) return;
	auto graphic = Lua.get_component(graphic_ptr);
	
	auto animation_ptr = entity->get_component("Animation");
	if (!check_component_exists(animation_ptr, "Animation")) return;
	auto animation = Lua.get_component(animation_ptr);

	// Get the sprite fron the Animation component, and make sure it exists
	auto& asset_manager = get_asset_manager();

	std::string sprite_name = animation["sprite"];
	auto sprite = asset_manager.get_asset<Sprite>(sprite_name);
	if (!sprite) {
		tdns_log.write("@draw_entity:nullptr_sprite: " + sprite_name);
		return;
	}
	if (!sprite->is_initialized()) {
		tdns_log.write("@draw_entity:uninitialized_sprite: " + sprite->name);
		return;
	}

	// Fill out the RenderElement
	Render_Element r;
	r.flags = static_cast<Render_Flags>(graphic["flags"]);
	r.layer = graphic["layer"];
	r.entity = id;
	r.sprite = sprite;

	auto& render_engine = get_render_engine();
	render_engine.render_list.push_back(r);
}

void API::register_position(int entity) {
	Position position;
	
	auto component = Lua.get_component(entity, "Position");
	position.x = component["world"]["x"];
	position.y = component["world"]["y"];

	auto& physics_engine = get_physics_engine();
	physics_engine.add_position(entity, position);
}

void API::register_collider(int entity) {
	auto& entity_manager = get_entity_manager();

	Collider collider;
	collider.entity = entity;

	auto entity_ptr = entity_manager.get_entity(entity);
	if (!entity_ptr->has_component("BoundingBox")) return;
	auto box = Lua.get_component(entity, "BoundingBox");
	
	collider.extents.x = box["extents"]["x"];
	collider.extents.y = box["extents"]["y"];
	collider.offset.x = box["offset"]["x"];
	collider.offset.y = box["offset"]["y"];	
	
	auto& physics_engine = get_physics_engine();
	physics_engine.add_collider(entity, collider);
}

void API::register_raycastable(int entity) {
	auto& entity_manager = get_entity_manager();

	Collider collider;
	collider.entity = entity;

	auto entity_ptr = entity_manager.get_entity(entity);
	if (!entity_ptr->has_component("BoundingBox")) return;
	auto box = Lua.get_component(entity, "BoundingBox");
	
	collider.extents.x = box["extents"]["x"];
	collider.extents.y = box["extents"]["y"];
	collider.offset.x = box["offset"]["x"];
	collider.offset.y = box["offset"]["y"];	
	
	auto& physics_engine = get_physics_engine();
	physics_engine.add_raycast(entity, collider);
}

void API::register_trigger(int entity) {
	auto& entity_manager = get_entity_manager();

	Collider collider;
	collider.entity = entity;

	auto entity_ptr = entity_manager.get_entity(entity);
	if (!entity_ptr->has_component("BoundingBox")) return;
	auto box = Lua.get_component(entity, "BoundingBox");
	
	collider.extents.x = box["extents"]["x"];
	collider.extents.y = box["extents"]["y"];
	collider.offset.x = box["offset"]["x"];
	collider.offset.y = box["offset"]["y"];	
	
	auto& physics_engine = get_physics_engine();
	physics_engine.add_trigger(entity, collider);
}

sol::object API::ray_cast(float x, float y) {
	auto& physics_engine = get_physics_engine();

	auto entity = physics_engine.ray_cast(x, y);
	if (entity >= 0) {
		return Lua.state["tdengine"]["entities"][entity];
	}

	return sol::make_object(Lua.state, sol::lua_nil);
}

void API::do_interaction_check() {
	auto& interaction = get_interaction_system();
	interaction.check_for_interactions = true;
}

void API::register_interactable(int entity) {
	Interactable interactable;
	interactable.entity = entity;

	auto box = Lua.get_component(entity, "BoundingBox");
	interactable.extents.x = box["extents"]["x"];
	interactable.extents.y = box["extents"]["y"];
	interactable.offset.x = box["offset"]["x"];
	interactable.offset.y = box["offset"]["y"];	

	auto& system = get_interaction_system();
	system.add_interactable(entity, interactable);
}

sol::object API::sprite_size(std::string name) {
	auto& asset_manager = get_asset_manager();
	auto sprite = asset_manager.get_asset<Sprite>(name);
	if (sprite) {
		auto size = Lua.state.create_table();
		size["x"] = sprite->width / internal_resolution_width;
		size["y"] = sprite->height / internal_resolution_height;
		return size;
	}

	return sol::make_object(Lua.raw_state, sol::lua_nil);
}

void API::move_entity_by_offset(int entity, float x, float y, int flags) {
	MoveRequest request;
	request.entity = entity;
	request.flags = flags;
	request.wish.x = x;
	request.wish.y = y;
		
	auto& physics_engine = get_physics_engine();
	physics_engine.requests.push_back(request);
}

void API::move_entity_absolute(int entity, float x, float y, int flags) {
	MoveRequest request;
	request.entity = entity;
	request.flags = flags | MoveFlags::AbsolutePosition;
	request.wish.x = x;
	request.wish.y = y;
		
	auto& physics_engine = get_physics_engine();
	physics_engine.requests.push_back(request);
}

void API::teleport_entity(int entity, float x, float y) {
	MoveRequest request;
	request.flags |= MoveFlags::BypassCollision;
	request.entity = entity;
	request.wish.x = x;
	request.wish.y = y;
		
	auto& physics_engine = get_physics_engine();
	physics_engine.requests.push_back(request);	
}

void API::register_animation(std::string name, std::vector<std::string> frames) {
	tdns_log.write("Registering animation: " + name, Log_Flags::File);
	
	Animation* animation = new Animation;
	for (auto& frame : frames) {
		animation->add_frame(frame);
	}

	auto& asset_manager = get_asset_manager();
	asset_manager.add_asset<Animation>(name, animation);
}

void API::enable_input_channel(int channel) {
	auto& input_manager = get_input_manager();
	input_manager.enable_channel(channel);
}

void API::disable_input_channel(int channel) {
	auto& input_manager = get_input_manager();
	input_manager.disable_channel(channel);
}

bool API::is_down(GLFW_KEY_TYPE id, int mask) {
	auto& manager = get_input_manager();
	if (!(manager.mask & mask)) return false;
	return manager.is_down[id];
}

bool API::was_pressed(GLFW_KEY_TYPE id, int mask) {
	auto& manager = get_input_manager();
	return manager.was_pressed(id, mask);
}

bool API::was_released(GLFW_KEY_TYPE id, int mask) {
	auto& manager = get_input_manager();
	return manager.was_released(id, mask);
}

bool API::was_chord_pressed(GLFW_KEY_TYPE mod_key, GLFW_KEY_TYPE cmd_key, int mask) {
	auto& manager = get_input_manager();
	return manager.chord(mod_key, cmd_key, mask);
}

sol::object API::cursor() {
	auto& manager = get_input_manager();
	
	auto out = Lua.state.create_table();
	out["x"] = manager.screen_pos.x;
	out["y"] = manager.screen_pos.y;
	return out;
}

sol::object API::screen_dimensions() {
	auto out = Lua.state.create_table();
	out["x"] = screen_x;
	out["y"] = screen_y;
	return out;
}

sol::object API::camera() {
	auto& renderer = get_render_engine();
	
	auto out = Lua.state.create_table();
	out["x"] = renderer.camera.x;
	out["y"] = renderer.camera.y;
	return out;
}

void API::move_camera(float x, float y) {
	auto& render_engine = get_render_engine();
	render_engine.camera.x = x;
	render_engine.camera.y = y;
}

void API::move_camera_by_offset(float x, float y) {
	auto& render_engine = get_render_engine();
	render_engine.camera.x += x;
	render_engine.camera.y += y;
}

bool API::draw_sprite_button(std::string sprite_name, float sx, float sy) {
	auto& asset_manager = get_asset_manager();
	auto sprite = asset_manager.get_asset<Sprite>(sprite_name);
	if (!sprite) {
		std::string message = "Tried to draw a SpriteButton, but couldn't get sprite.\n";
		message += "Sprite name was: " + sprite_name;
		tdns_log.write(message);
		return false;
	}

	ImVec2 top_right_tex_coords = ImVec2(sprite->tex_coords[2], sprite->tex_coords[3]);
	ImVec2 bottom_left_tex_coords = ImVec2(sprite->tex_coords[6], sprite->tex_coords[7]);
	ImVec2 button_size = ImVec2(sx, sy);

	return ImGui::ImageButton((ImTextureID)(uintptr_t)sprite->atlas->handle,
							  button_size,
							  bottom_left_tex_coords, top_right_tex_coords);
}

void API::line_screen(sol::table p1, sol::table p2, sol::table color) {
	glm::vec2 a{p1["x"], p2["y"]};
	glm::vec2 b{p2["x"], p2["y"]};
	glm::vec4 clr{color["r"], color["g"], color["b"], color["a"]};
	draw_line_from_points(a, b, clr);
}

void API::rect_filled_screen(sol::table rect, sol::table color) {
	glm::vec2 origin{rect["origin"]["x"], rect["origin"]["y"]};
	glm::vec2 extents{rect["extents"]["x"], rect["extents"]["y"]};
	glm::vec4 clr{color["r"], color["g"], color["b"], color["a"]};
	draw_rect_filled_screen(origin, extents, clr);
}

void API::rect_outline_screen(sol::table rect, sol::table color) {
	glm::vec2 origin{rect["origin"]["x"], rect["origin"]["y"]};
	glm::vec2 extents{rect["extents"]["x"], rect["extents"]["y"]};
	glm::vec4 clr{color["r"], color["g"], color["b"], color["a"]};
	draw_rect_outline_screen(origin, extents, clr);
}

void API::rect_outline_world(sol::table rect, sol::table color) {
	glm::vec2 origin{rect["origin"]["x"], rect["origin"]["y"]};
	glm::vec2 extents{rect["extents"]["x"], rect["extents"]["y"]};
	glm::vec4 clr{color["r"], color["g"], color["b"], color["a"]};
	draw_rect_outline_world(origin, extents, clr);
}

void API::toggle_console() {
	show_console = !show_console;
}

void API::use_layout(const char* name) {
	layout_to_load = name;
}

void API::save_layout(const char* name) {
	auto relative = RelativePath(std::string("layouts/") + name + ".ini");
	auto layout = ScriptPath(relative);
	ImGui::SaveIniSettingsToDisk(layout.path.c_str());
	tdns_log.write("Saved Imgui configuration: " + layout.path, Log_Flags::File);
}

void API::draw_text(std::string text, float x, float y, int flags) {
	glm::vec2 point(x, y);
	draw_text(text, point, static_cast<Text_Flags>(flags));
}

void API::screen(const char* dimension) {
	if (!strcmp(dimension, "640")) use_640_360();
	else if (!strcmp(dimension, "720")) use_720p();
	else if (!strcmp(dimension, "1080")) use_1080p();
	else if (!strcmp(dimension, "1440")) use_1440p();
}

void API::log(const char* message) {
	tdns_log.write(message);
}

void API::log_to(const char* message, uint8_t flags) {
	tdns_log.write(message, flags);
}

void API::use_step_mode() {
	step_mode = true;
	show_console = false;
}

void API::fade_screen(float time) {
	auto& render_engine = get_render_engine();
	render_engine.render_to_frame_buffer = true;
	render_engine.fade_time = time;
	render_engine.fade_time_remaining = time * 2;
}

void API::snap_to_player() {
	auto& render_engine = get_render_engine();
	render_engine.camera.snap_to_player = true;
}

void API::follow_player(bool follow) {
	auto& render_engine = get_render_engine();
	render_engine.camera.follow_player = follow;
}

bool API::is_following_player() {
	auto& render_engine = get_render_engine();
	return render_engine.camera.follow_player;
}

void API::pause_updates() {
	are_updates_paused = true;
}

void API::resume_updates() {
	are_updates_paused = false;
}

void API::set_imgui_demo(bool show) {
	show_imgui_demo = show;
}

void register_lua_api() {
	auto& state = Lua.state;
	
	using namespace API;
    state["tdengine"]                          = state.create_table();
	state["tdengine"]["alloc_entity"]          = &alloc_entity;
	state["tdengine"]["free_entity"]           = &free_entity;
	state["tdengine"]["move_entity_by_offset"] = &move_entity_by_offset;
	state["tdengine"]["move_entity_absolute"]  = &move_entity_absolute;
	state["tdengine"]["entity_name"]           = &entity_name;
	state["tdengine"]["has_component"]         = &has_component;
	state["tdengine"]["get_component"]         = &get_component;
	state["tdengine"]["add_component"]         = &add_component;
	state["tdengine"]["all_components"]        = &all_components;
	state["tdengine"]["alloc_component"]       = &alloc_component;
	state["tdengine"]["free_component"]        = &free_component;
	state["tdengine"]["component_name"]        = &component_name;
	state["tdengine"]["register_animation"]    = &register_animation;
	state["tdengine"]["enable_input_channel"]  = &enable_input_channel;
	state["tdengine"]["disable_input_channel"] = &disable_input_channel;
	state["tdengine"]["is_down"]               = &is_down;
	state["tdengine"]["was_pressed"]           = &was_pressed;
	state["tdengine"]["was_released"]          = &was_pressed;
	state["tdengine"]["was_chord_pressed"]     = &was_chord_pressed;
	state["tdengine"]["cursor"]                = &cursor;
	state["tdengine"]["camera"]                = &camera;
	state["tdengine"]["move_camera"]           = &move_camera;
	state["tdengine"]["move_camera_by_offset"] = &move_camera_by_offset;
	state["tdengine"]["sprite_size"]           = &sprite_size;
	state["tdengine"]["register_position"]     = &register_position;
	state["tdengine"]["register_collider"]     = &register_collider;
	state["tdengine"]["register_raycastable"]  = &register_raycastable;
	state["tdengine"]["register_trigger"]      = &register_trigger;
	state["tdengine"]["register_interactable"] = &register_interactable;
	state["tdengine"]["do_interaction_check"]  = &do_interaction_check;
	state["tdengine"]["register_player"]       = &register_player;
	state["tdengine"]["teleport_entity"]       = &teleport_entity;
	state["tdengine"]["save_layout"]           = &save_layout;
	state["tdengine"]["use_layout"]            = &use_layout;
	state["tdengine"]["frame_time"]            = seconds_per_update;
	state["tdengine"]["screen_dimensions"]     = &screen_dimensions;
	state["tdengine"]["ray_cast"]              = &ray_cast;
	state["tdengine"]["screen"]                = &screen;
	state["tdengine"]["step_mode"]             = &use_step_mode;
	state["tdengine"]["fade_screen"]           = &fade_screen;
	state["tdengine"]["snap_to_player"]        = &snap_to_player;
	state["tdengine"]["follow_player"]         = &follow_player;
	state["tdengine"]["is_following_player"]   = &is_following_player;
	state["tdengine"]["log"]                   = &API::log;
	state["tdengine"]["log_to"]                = &API::log_to;
	state["tdengine"]["toggle_console"]        = &toggle_console;
	state["tdengine"]["pause_updates"]         = &API::pause_updates;
	state["tdengine"]["resume_updates"]        = &resume_updates;
	state["tdengine"]["set_imgui_demo"]        = &set_imgui_demo;

	state["tdengine"]["draw"]                        = state.create_table();
	state["tdengine"]["draw"]["entity"]              = &draw_entity;	
	state["tdengine"]["draw"]["line_screen"]         = &line_screen;
	state["tdengine"]["draw"]["rect_filled_screen"]  = &rect_filled_screen;
	state["tdengine"]["draw"]["rect_outline_screen"] = &rect_outline_screen;
	state["tdengine"]["draw"]["rect_outline_world"]  = &rect_outline_world;	
	state["tdengine"]["draw"]["rect_outline_world"]  = &rect_outline_world;	
	state["tdengine"]["draw"]["text"]                = &API::draw_text;
	
	state["tdengine"]["font"]                        = state.create_table();
	state["tdengine"]["font"]["advance"]             = &advance;

	state["tdengine"]["paths"] = state.create_table();
	state["tdengine"]["paths"]["root"] = root_dir;
	state["tdengine"]["paths"]["join"] = &path_join;
	state["tdengine"]["paths"]["absolute"] = &absolute_path;	
	
	state["tdengine"]["flags"] = state.create_table();
	
	state["tdengine"]["flags"]["physics"] = state.create_table();
	state["tdengine"]["flags"]["physics"]["bypass"] = MoveFlags::BypassCollision;
	state["tdengine"]["flags"]["physics"]["absolute"] = MoveFlags::AbsolutePosition;

	
	state["tdengine"]["text_flags"] = state.create_table();
	state["tdengine"]["text_flags"]["none"] = Text_Flags::None;
	state["tdengine"]["text_flags"]["highlighted"] = Text_Flags::Highlighted;
	state["tdengine"]["render_flags"]= state.create_table();	
	state["tdengine"]["render_flags"]["none"] = Render_Flags::None;	
	state["tdengine"]["render_flags"]["highlighted"] = Render_Flags::Highlighted;	
	state["tdengine"]["render_flags"]["screen_position"] = Render_Flags::ScreenPosition;	
	state["tdengine"]["log_flags"]= state.create_table();	
	state["tdengine"]["log_flags"]["console"] = Log_Flags::Console;	
	state["tdengine"]["log_flags"]["file"] = Log_Flags::File;	
	state["tdengine"]["log_flags"]["default"] = Log_Flags::Default;	

	state["tdengine"]["InputChannel"] = state.create_table();
	state["tdengine"]["InputChannel"]["None"] = INPUT_MASK_NONE;
	state["tdengine"]["InputChannel"]["ImGui"] = INPUT_MASK_IMGUI;
	state["tdengine"]["InputChannel"]["Editor"] = INPUT_MASK_EDITOR;
	state["tdengine"]["InputChannel"]["Game"] = INPUT_MASK_GAME;
	state["tdengine"]["InputChannel"]["Player"] = INPUT_MASK_PLAYER;
	state["tdengine"]["InputChannel"]["All"] = INPUT_MASK_ALL;

		
	state["imgui"]["extensions"] = state.create_table();
	state["imgui"]["extensions"]["SpriteButton"] = &draw_sprite_button;	
}
