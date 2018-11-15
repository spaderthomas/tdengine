// Level 
Level* get_level(string name) {
	return levels[name];
}
void add_entity(Level* level, EntityHandle entity) {
	level->entity_handles.push_back(entity);
}

// Entity
string entity_name(EntityHandle entity) {
	return entity->lua_id;
}
int entity_id(EntityHandle entity) {
	return entity->id;
}
void on_collide(EntityHandle me, EntityHandle other) {
	def_get_cmp(cc, me, Collision_Component);
	if (cc)
		cc->on_collide(me, other);
}
void update_animation(EntityHandle me, float dt) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	auto anim = gc->active_animation;
	anim->time_to_next_frame -= dt;
	if (anim->time_to_next_frame <= 0.f) {
		anim->next_frame();
	}
}
int collider_kind(EntityHandle me) {
	def_get_cmp(cc, me.deref(), Collision_Component);
	return cc ?
		cc->kind :
		Collider_Kind::NO_COLLIDER;
}
void draw_entity(EntityHandle me, Render_Flags flags) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	def_get_cmp(pc, me.deref(), Position_Component);
	if (gc && pc) {
		renderer.draw(gc, pc, flags);
	}
}
void teleport_entity(EntityHandle me, float x, float y) {
	def_get_cmp(pc, me.deref(), Position_Component);
	if (pc)
		pc->world_pos = { x, y };
}
void move_entity(EntityHandle me, float dx, float dy) {
	def_get_cmp(mc, me.deref(), Movement_Component);
	mc->wish += glm::vec2(dx, dy);

	// Tell the Physics system to check + resolve if this movement causes collisions
	physics_system.movers.push_back(me);
}
void set_animation(EntityHandle me, string wish_name) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	for (auto& animation : gc->animations) {
		if (animation->name == wish_name) {
			gc->active_animation = animation;
			gc->active_animation->icur_frame = 0;
			return;
		}
	}

	string msg = "Tried to set active animation to " + wish_name + " but it was not registered in the component!";
	tdns_log.write(msg);
}
void set_animation_no_reset(EntityHandle me, string wish_name) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	if (gc->active_animation) {
		if (gc->active_animation->name == wish_name) {
			return;
		}
	}

	set_animation(me, wish_name);
}
void update_entity(EntityHandle me, float dt) {
	def_get_cmp(uc, me.deref(), Update_Component);
	if (uc)
		uc->update(me, dt);
}
bool are_interacting(EntityHandle initiator, EntityHandle receiver) {
	// Check if the initiator's vision box collides with the receiver's bounding box
	def_get_cmp(cc, receiver.deref(), Collision_Component);
	def_get_cmp(receiver_pc, receiver.deref(), Position_Component);
	def_get_cmp(following, camera.following.deref(), Position_Component);
	glm::vec2 penetration;

	if (debug_show_aabb) {
		Points_Box vision_box = get_vision_box(initiator);
		Points_Box receiver_box = get_bounding_box_world(receiver);
		draw_rect_world(vision_box, red);
		if (are_boxes_colliding(get_vision_box(initiator), get_bounding_box_world(receiver), penetration)) {
			draw_rect_world(get_bounding_box_world(receiver), green);
			return true;
		}

		return false;
	}
	else {
		return are_boxes_colliding(get_vision_box(initiator), get_bounding_box_world(receiver), penetration);
	}
}

// Input
bool lua_is_down(GLFW_KEY_TYPE key) {
	return game.input.is_down[key];
}
bool lua_was_down(GLFW_KEY_TYPE key) {
	return game.input.was_down[key];
}
bool lua_was_pressed(GLFW_KEY_TYPE key) {
	return game.input.was_pressed(key);
}

// Game
void go_through_door(string dest_level) {
	Lua.state["game"]["level"] = levels[dest_level];
}
void begin_dialogue(EntityHandle entity, string scene) {
	string npc = entity->lua_id;
	game.active_dialogue->init_from_table(npc, scene);
	Lua.state["game"]["state"] = Game_State::DIALOGUE;
}
