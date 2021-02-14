namespace API {
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

	// Get the sprite for the requested frame
	std::string animation_name = animation["current"];
	int frame = animation["frame"];

	auto& asset_manager = get_asset_manager();
	Animation* animation_p = asset_manager.get_asset<Animation>(animation_name);
	if (!animation_p) {
		tdns_log.write("Could not find animation: " + animation_name);
		return;
	}

	auto sprite_name = animation_p->get_frame(frame);
	auto sprite = asset_manager.get_asset<Sprite>(sprite_name);
	if (!sprite) {
		tdns_log.write("Could not find sprite: " + sprite_name);
		return;
	}

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
		tdns_log.write(message, Log_Flags::File);

		collider.extents.x = 0;
		collider.extents.y = 0;	
	} else {
		auto box = Lua.get_component(entity, "BoundingBox");
		collider.extents.x = box["extents"]["x"];
		collider.extents.y = box["extents"]["y"];
		collider.offset.x = box["offset"]["x"];
		collider.offset.y = box["offset"]["y"];	
	}
	
	auto position = Lua.get_component(entity, "Position");
	collider.origin.x = position["world"]["x"];
	collider.origin.y = position["world"]["y"];
	
	auto& physics_engine = get_physics_engine();
	physics_engine.add_collider(entity, collider);
}

sol::object ray_cast(float x, float y) {
	auto& physics_engine = get_physics_engine();

	for (const auto& [entity, collider] : physics_engine.colliders) {
		if (point_inside_collider(x, y, collider)) {
			return Lua.state["tdengine"]["entities"][entity];
		}
	}

	return sol::make_object(Lua.state, sol::lua_nil);
}

sol::object sprite_size(std::string name) {
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



void register_animation(std::string name, std::vector<std::string> frames) {
	tdns_log.write("Registering animation: " + name, Log_Flags::File);
	
	Animation* animation = new Animation;
	for (auto& frame : frames) {
		animation->add_frame(frame);
	}

	auto& asset_manager = get_asset_manager();
	asset_manager.add_asset<Animation>(name, animation);
}

int count_frames(std::string animation) {
	auto& asset_manager = get_asset_manager();
	Animation* asset = asset_manager.get_asset<Animation>(animation);
	if (!asset) {
		tdns_log.write("Asked for animation's frames, but could not find that animation");
		tdns_log.write("Animation name: " + animation);
		return 0;
	}
	
	return asset->frames.size();
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

sol::object cursor() {
	auto& manager = get_input_manager();
	
	auto out = Lua.state.create_table();
	out["x"] = manager.screen_pos.x;
	out["y"] = manager.screen_pos.y;
	return out;
}

sol::object screen_dimensions() {
	auto out = Lua.state.create_table();
	out["x"] = screen_x;
	out["y"] = screen_y;
	return out;
}

sol::object camera() {
	auto& renderer = get_render_engine();
	
	auto out = Lua.state.create_table();
	out["x"] = renderer.camera.x;
	out["y"] = renderer.camera.y;
	return out;
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

void draw_text(std::string text, float x, float y, int flags) {
	glm::vec2 point(x, y);
	draw_text(text, point, static_cast<Text_Flags>(flags));
}
void screen(const char* dimension) {
	if (!strcmp(dimension, "640")) use_640_360();
	else if (!strcmp(dimension, "720")) use_720p();
	else if (!strcmp(dimension, "1080")) use_1080p();
	else if (!strcmp(dimension, "1440")) use_1440p();
}

void log(const char* message, uint8_t flags) {
	tdns_log.write(message, flags);
}

}
	
void register_lua_api() {
	auto& state = Lua.state;
	
	using namespace API;
    state["tdengine"] = state.create_table();
	state["tdengine"]["create_entity"] = &create_entity;
	state["tdengine"]["destroy_entity"] = &destroy_entity;
	state["tdengine"]["register_animation"] = &register_animation;
	state["tdengine"]["count_frames"] = &count_frames;
	state["tdengine"]["enable_input_channel"] = &enable_input_channel;
	state["tdengine"]["disable_input_channel"] = &disable_input_channel;
	state["tdengine"]["is_down"] = &is_down;
	state["tdengine"]["was_pressed"] = &was_pressed;
	state["tdengine"]["was_chord_pressed"] = &was_chord_pressed;
	state["tdengine"]["cursor"] = &cursor;
	state["tdengine"]["camera"] = &camera;
	state["tdengine"]["move_camera"] = &move_camera;
	state["tdengine"]["sprite_size"] = &sprite_size;	
	state["tdengine"]["teleport_entity"] = &teleport_entity;
	state["tdengine"]["register_collider"] = &register_collider;
	state["tdengine"]["save_layout"] = &save_layout;
	state["tdengine"]["use_layout"] = &use_layout;
	state["tdengine"]["frame_time"] = seconds_per_update;
	state["tdengine"]["screen_dimensions"] = &screen_dimensions;
	state["tdengine"]["ray_cast"] = &ray_cast;
	state["tdengine"]["screen"] = &screen;
	state["tdengine"]["log"] = &API::log;

	state["tdengine"]["draw"] = state.create_table();
	state["tdengine"]["draw"]["entity"] = &draw_entity;	
	state["tdengine"]["draw"]["line_screen"] = &line_screen;
	state["tdengine"]["draw"]["rect_filled_screen"] = &rect_filled_screen;
	state["tdengine"]["draw"]["rect_outline_screen"] = &rect_outline_screen;
	state["tdengine"]["draw"]["rect_outline_world"] = &rect_outline_world;	
	state["tdengine"]["draw"]["rect_outline_world"] = &rect_outline_world;	
	state["tdengine"]["draw"]["text"] = &API::draw_text;

	state["tdengine"]["font"] = state.create_table();
	state["tdengine"]["font"]["advance"] = &advance;

	state["tdengine"]["internal"] = state.create_table();
	state["tdengine"]["internal"]["move_entity"] = &move_entity;
	state["tdengine"]["internal"]["toggle_console"] = &toggle_console;

	state["tdengine"]["paths"] = state.create_table();
	state["tdengine"]["paths"]["root"] = root_dir;
	state["tdengine"]["paths"]["join"] = &path_join;
	state["tdengine"]["paths"]["absolute"] = &absolute_path;	
	
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
	state["tdengine"]["InputChannel"]["All"] = INPUT_MASK_ALL;

		
	state["imgui"]["extensions"] = state.create_table();
	state["imgui"]["extensions"]["SpriteButton"] = &draw_sprite_button;	
}
