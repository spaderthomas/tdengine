int create_entity(std::string name) {
	auto& entity_manager = get_entity_manager();
	return entity_manager.create_entity(name);
}

void destroy_entity(int entity) {
	auto& entity_manager = get_entity_manager();
	auto& physics_engine = get_physics_engine();
	auto& render_engine = get_render_engine();
	render_engine.remove_entity(entity);
	physics_engine.remove_entity(entity);
	entity_manager.destroy_entity(entity);
}

void draw_entity(int entity) {
	auto& entity_manager = get_entity_manager();
	auto entity_ptr = entity_manager.get_entity(entity);
	
	// Grab the relevant components
	auto check_component_exists = [entity_ptr](auto component, std::string component_type) -> bool {
		if (component) return true;
		
		tdns_log.write("Called draw_entity(), but entity had no" + component_type + " component");
		tdns_log.write("Entity name: " + entity_ptr->get_name());
		tdns_log.write("Entity ID: " + entity_ptr->get_id());
		return false;
	};
	
	auto graphic_ptr = entity_ptr->get_component("Graphic");
	if (!check_component_exists(graphic_ptr, "Graphic")) return;
	auto graphic = Lua.get_component(graphic_ptr);
	
	auto animation_ptr = entity_ptr->get_component("Animation");
	if (!check_component_exists(animation_ptr, "Animation")) return;
	auto animation = Lua.get_component(animation_ptr);

	// Fill out the RenderElement
	Render_Element r;
	r.flags = static_cast<Render_Flags>(graphic["flags"]);
	r.layer = graphic["layer"];
	r.entity = entity;

	std::string animation_name = animation["current"];
	int frame = animation["frame"];
	Sprite* sprite = get_frame(animation_name, frame);
	if (!sprite) {
		tdns_log.write("Trying to render, but sprite returned was invalid (nullptr).");
		tdns_log.write("Animation was: " + animation_name);
		tdns_log.write("Frame was: " + std::to_string(frame));
		return;
	}
	if (!sprite->is_initialized()) {
		tdns_log.write("Trying to render, but the sprite was uninitialized. Sprite was: " + sprite->name);
		return;
	}

	r.sprite = sprite;

	auto& render_engine = get_render_engine();
	render_engine.render_list.push_back(r);
}

void register_collider(int entity) {
	auto& entity_manager = get_entity_manager();

	Collider collider;
	collider.entity = entity;

	auto entity_ptr = entity_manager.get_entity(entity);
	if (!entity_ptr->has_component("BoundingBox")) {
		std::string message = "Called register_collider() for ";
		message += entity_ptr->debug_string() + " but it did not have a BoundingBox";
		message += ". Using default bounding box extents.";
		tdns_log.write(message);

		collider.extents.x = 0;
		collider.extents.y = 0;	
	} else {
		auto box = Lua.get_component(entity, "BoundingBox");
		collider.extents.x = box["extents"]["x"];
		collider.extents.y = box["extents"]["y"];	
	}
	
	auto position = Lua.get_component(entity, "Position");
	collider.origin.x = position["world"]["x"];
	collider.origin.y = position["world"]["y"];
	
	auto& physics_engine = get_physics_engine();
	physics_engine.add_collider(entity, collider);

	tdns_log.write("Registered collider for EntityID " + entity_ptr->debug_string());
}

int ray_cast(float x, float y) {
	auto& physics_engine = get_physics_engine();

	for (const auto& [entity, collider] : physics_engine.colliders) {
		if (point_inside_collider(x, y, collider)) {
			return entity;
		}
	}

	return -1;
}

void move_entity(int entity) {
	auto box = Lua.get_component(entity, "BoundingBox");
	auto movement = Lua.get_component(entity, "Movement");

	MoveRequest request;
	request.entity = entity;

	request.wish.x = movement["wish"]["x"];
	request.wish.y = movement["wish"]["y"];
		
	auto& physics_engine = get_physics_engine();
	physics_engine.requests.push_back(request);
}

void teleport_entity(int entity, float x, float y) {
	MoveRequest request;
	request.flags |= MoveFlags::BypassCollision;
	request.entity = entity;
	request.wish.x = x;
	request.wish.y = y;
		
	auto& physics_engine = get_physics_engine();
	physics_engine.requests.push_back(request);	
}

Sprite* get_frame(std::string animation, int frame) {
	if (animation.empty()) {
		tdns_log.write("Component asked for current frame but no active animation was set!");
		return (Sprite*)nullptr;
	}
	if (frame < 0) {
		std::string msg = "Frame index less than 0 for animation " + animation;
		tdns_log.write(msg);
		return (Sprite*)nullptr;
	}

	auto& asset_manager = get_asset_manager();
	Animation* animation_p = asset_manager.get_asset<Animation>(animation);
	if (!animation_p) {
		tdns_log.write("Could not find animation: " + animation);
		return (Sprite*)nullptr;
	}

	auto sprite_name = animation_p->get_frame(frame);
	auto frame_p = asset_manager.get_asset<Sprite>(sprite_name);
	if (!frame_p) {
		tdns_log.write("Could not find sprite: " + sprite_name);
		return (Sprite*)nullptr;
	}

	return frame_p;
}

void register_animation(std::string name, std::vector<std::string> frames) {
	tdns_log.write("Registering animation: " + name, Log_Flags::File);
	
	Animation* animation = new Animation;
	for (auto& frame : frames) {
		animation->add_frame(frame);
	}

	auto& asset_manager = get_asset_manager();
	asset_manager.add_asset<Animation>(name, animation);
}

std::vector<std::string> get_frames(std::string animation_name) {
	auto& asset_manager = get_asset_manager();
	Animation* animation = asset_manager.get_asset<Animation>(animation_name);
	if (!animation) {
		tdns_log.write("Asked for animation's frames, but could not find that animation");
		tdns_log.write("Animation name: " + animation_name);
		return {};
	}
	
	return animation->frames;
}

void enable_input_channel(int channel) {
	auto& input_manager = get_input_manager();
	input_manager.enable_channel(channel);
}

void disable_input_channel(int channel) {
	auto& input_manager = get_input_manager();
	input_manager.disable_channel(channel);
}

bool is_down(GLFW_KEY_TYPE id, int mask) {
	auto& manager = get_input_manager();
	if (!(manager.mask & mask)) return false;
	return manager.is_down[id];
}

bool was_pressed(GLFW_KEY_TYPE id, int mask) {
	auto& manager = get_input_manager();
	return manager.was_pressed(id, mask);
}
bool was_chord_pressed(GLFW_KEY_TYPE mod_key, GLFW_KEY_TYPE cmd_key, int mask) {
	auto& manager = get_input_manager();
	return manager.chord(mod_key, cmd_key, mask);
}
float get_cursor_x() {
	auto& manager = get_input_manager();
	return manager.screen_pos.x;
}
float get_cursor_y() {
	auto& manager = get_input_manager();
	return manager.screen_pos.y;
}

float get_camera_x() {
	auto& render_engine = get_render_engine();
	return render_engine.camera.x;
}
float get_camera_y() {
	auto& render_engine = get_render_engine();
	return render_engine.camera.y;
}
void move_camera(float x, float y) {
	auto& render_engine = get_render_engine();
	render_engine.camera.x += x;
	render_engine.camera.y += y;
}

bool draw_sprite_button(std::string sprite_name, float sx, float sy) {
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

void line_screen(sol::table p1, sol::table p2, sol::table color) {
	glm::vec2 a{p1["x"], p2["y"]};
	glm::vec2 b{p2["x"], p2["y"]};
	glm::vec4 clr{color["r"], color["g"], color["b"], color["a"]};
	draw_line_from_points(a, b, clr);
}

void rect_filled_screen(sol::table rect, sol::table color) {
	glm::vec2 origin{rect["origin"]["x"], rect["origin"]["y"]};
	glm::vec2 extents{rect["extents"]["x"], rect["extents"]["y"]};
	glm::vec4 clr{color["r"], color["g"], color["b"], color["a"]};
	draw_rect_filled_screen(origin, extents, clr);
}

void rect_outline_screen(sol::table rect, sol::table color) {
	glm::vec2 origin{rect["origin"]["x"], rect["origin"]["y"]};
	glm::vec2 extents{rect["extents"]["x"], rect["extents"]["y"]};
	glm::vec4 clr{color["r"], color["g"], color["b"], color["a"]};
	draw_rect_outline_screen(origin, extents, clr);
}

void rect_outline_world(sol::table rect, sol::table color) {
	glm::vec2 origin{rect["origin"]["x"], rect["origin"]["y"]};
	glm::vec2 extents{rect["extents"]["x"], rect["extents"]["y"]};
	glm::vec4 clr{color["r"], color["g"], color["b"], color["a"]};
	draw_rect_outline_world(origin, extents, clr);
}

void toggle_console() {
	show_console = !show_console;
}

void use_layout(const char* name) {
	layout_to_load = name;
}

void save_layout(const char* name) {
	auto relative = RelativePath(std::string("layouts/") + name + ".ini");
	auto layout = ScriptPath(relative);
	ImGui::SaveIniSettingsToDisk(layout.path.c_str());
	tdns_log.write("Saved Imgui configuration: " + layout.path, Log_Flags::File);
}

void save_imgui_layout() {
	ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename);
}

void text_box_begin(std::string text) {
	auto& text_box = get_text_box();
	// hack to get choices working with minimal code changes
	if (text.empty()) {
		text_box.begin(text_box.text);
	} else {
		text_box.begin(std::move(text));
	}
}
bool text_box_is_done() {
	auto& text_box = get_text_box();
	return text_box.is_done();
}
bool text_box_is_active() {
	auto& text_box = get_text_box();
	return text_box.active;
}
bool text_box_is_waiting() {
	auto& text_box = get_text_box();
	return text_box.waiting;
}
void text_box_resume() {
	auto& text_box = get_text_box();
	text_box.resume();	
}
void text_box_skip() {
	auto& text_box = get_text_box();
	text_box.skip();	
}
void text_box_highlight_line(int line) {
	auto& text_box = get_text_box();
	text_box.line_to_highlight = line;	
}
void text_box_add_choice(std::string choice) {
	auto& text_box = get_text_box();
	text_box.add_choice(choice);
}
void text_box_clear() {
	auto& text_box = get_text_box();
	text_box.clear();	
}

void register_lua_api() {
	auto& state = Lua.state;
	
    state["tdengine"] = state.create_table();
	state["tdengine"]["create_entity"] = &create_entity;
	state["tdengine"]["destroy_entity"] = &destroy_entity;
	state["tdengine"]["register_animation"] = &register_animation;
	state["tdengine"]["get_frames"] = &get_frames;
	state["tdengine"]["enable_input_channel"] = &enable_input_channel;
	state["tdengine"]["disable_input_channel"] = &disable_input_channel;
	state["tdengine"]["is_down"] = &is_down;
	state["tdengine"]["was_pressed"] = &was_pressed;
	state["tdengine"]["was_chord_pressed"] = &was_chord_pressed;
	state["tdengine"]["get_cursor_x"] = &get_cursor_x;
	state["tdengine"]["get_cursor_y"] = &get_cursor_y;
	state["tdengine"]["get_camera_x"] = &get_camera_x;
	state["tdengine"]["get_camera_y"] = &get_camera_y;
	state["tdengine"]["move_camera"] = &move_camera;
	state["tdengine"]["draw_entity"] = &draw_entity;	
	state["tdengine"]["draw"] = state.create_table();
	state["tdengine"]["draw"]["line_screen"] = &line_screen;
	state["tdengine"]["draw"]["rect_filled_screen"] = &rect_filled_screen;
	state["tdengine"]["draw"]["rect_outline_screen"] = &rect_outline_screen;
	state["tdengine"]["draw"]["rect_outline_world"] = &rect_outline_world;	
	state["tdengine"]["render_flags"]= state.create_table();	
	state["tdengine"]["render_flags"]["none"] = Render_Flags::None;	
	state["tdengine"]["render_flags"]["highlighted"] = Render_Flags::Highlighted;	
	state["tdengine"]["render_flags"]["screen_position"] = Render_Flags::ScreenPosition;	
	state["tdengine"]["draw"]["rect_outline_world"] = &rect_outline_world;	
	state["tdengine"]["draw"]["rect_outline_world"] = &rect_outline_world;	
	state["tdengine"]["text_box"] = state.create_table();	
	state["tdengine"]["text_box"]["begin"] = &text_box_begin;	
	state["tdengine"]["text_box"]["is_done"] = &text_box_is_done;	
	state["tdengine"]["text_box"]["is_active"] = &text_box_is_active;	
	state["tdengine"]["text_box"]["is_waiting"] = &text_box_is_waiting;	
	state["tdengine"]["text_box"]["resume"] = &text_box_resume;	
	state["tdengine"]["text_box"]["skip"] = &text_box_skip;	
	state["tdengine"]["text_box"]["highlight_line"] = &text_box_highlight_line;	
	state["tdengine"]["text_box"]["add_choice"] = &text_box_add_choice;	
	state["tdengine"]["text_box"]["clear"] = &text_box_clear;	
	state["tdengine"]["InputChannel"] = state.create_table();
	state["tdengine"]["InputChannel"]["None"] = INPUT_MASK_NONE;
	state["tdengine"]["InputChannel"]["ImGui"] = INPUT_MASK_IMGUI;
	state["tdengine"]["InputChannel"]["Editor"] = INPUT_MASK_EDITOR;
	state["tdengine"]["InputChannel"]["Game"] = INPUT_MASK_GAME;
	state["tdengine"]["InputChannel"]["All"] = INPUT_MASK_ALL;
	state["tdengine"]["paths"] = state.create_table();
	state["tdengine"]["paths"]["root"] = root_dir;
	state["tdengine"]["paths"]["join"] = &path_join;
	state["tdengine"]["paths"]["absolute_path"] = &absolute_path;	
    state["tdengine"]["internal"] = state.create_table();
	state["tdengine"]["internal"]["move_entity"] = &move_entity;
	state["tdengine"]["teleport_entity"] = &teleport_entity;
	state["tdengine"]["register_collider"] = &register_collider;
	state["tdengine"]["internal"]["screen_640"] = &use_640_360;
	state["tdengine"]["internal"]["screen_720"] = &use_720p;
	state["tdengine"]["internal"]["screen_1080"] = &use_1080p;
	state["tdengine"]["internal"]["screen_1440"] = &use_1440p;
	state["tdengine"]["internal"]["toggle_console"] = &toggle_console;
	state["tdengine"]["save_layout"] = &save_layout;
	state["tdengine"]["use_layout"] = &use_layout;
	// This one's internal because we want the Lua version to return a table
	state["tdengine"]["internal"]["ray_cast"] = &ray_cast;
	state["imgui"]["extensions"] = state.create_table();
	state["imgui"]["extensions"]["SpriteButton"] = &draw_sprite_button;
	state.set_function("tdengine.log", &Log::write, &tdns_log);
}
