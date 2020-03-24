void create_entity(std::string name) {
	auto& entity_manager = get_entity_manager();
	entity_manager.create_entity(name);
}

void draw_entity(int entity, Render_Flags flags) {
	EntityHandle handle;
	handle.id = entity;
	if (!handle) return;

	// Grab the relevant components
	auto check_component_exists = [handle](auto component, std::string component_type) -> bool {
		if (component) return true;
		
		tdns_log.write("Called draw_entity(), but entity had no" + component_type + " component");
		tdns_log.write("Entity name: " + handle->get_name());
		tdns_log.write("Entity ID: " + handle->get_id());
		return false;
	};
	
	auto graphic_ptr = handle->get_component("Graphic");
	if (!check_component_exists(graphic_ptr, "Graphic")) return;
	auto graphic = Lua.get_component(graphic_ptr);
	
	auto position_ptr = handle->get_component("Position");
	if (!check_component_exists(position_ptr, "Position")) return;
	auto position = Lua.get_component(position_ptr);
	
	auto animation_ptr = handle->get_component("Animation");
	if (!check_component_exists(animation_ptr, "Animation")) return;
	auto animation = Lua.get_component(animation_ptr);

	// Fill out the RenderElement
	Render_Element r;
	r.flags = flags;
	r.layer = graphic["layer"];
	r.world_pos[0] = position["world"]["x"];
	r.world_pos[1] = position["world"]["y"];
	r.scale[0] = graphic["scale"]["x"];
	r.scale[1] = graphic["scale"]["y"];

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

void move_entity(int entity) {
	EntityHandle handle;
	handle.id = entity;
	if (!handle) return;

	//auto physics_engine = get_physics_engine();
	//physics_engine.add_mover(entity);
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

bool is_key_down(GLFW_KEY_TYPE id, int mask) {
	auto& manager = get_input_manager();
	if (!(manager.mask & mask)) return false;
	return manager.is_down[id];
}

bool was_key_pressed(GLFW_KEY_TYPE id, int mask) {
	auto& manager = get_input_manager();
	return manager.was_pressed(id, mask);
}

bool was_chord_pressed(GLFW_KEY_TYPE mod_key, GLFW_KEY_TYPE cmd_key, int mask) {
	auto& manager = get_input_manager();
	return manager.chord(mod_key, cmd_key, mask);
}

void set_camera_offset(float x, float y) {
	auto& render_engine = get_render_engine();
	render_engine.camera_offset = { x, y };
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
