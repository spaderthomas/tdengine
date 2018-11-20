// Level 
tdapi Level* get_level(string name) {
	return levels[name];
}
tdapi void add_entity(Level* level, EntityHandle entity) {
	level->entity_handles.push_back(entity);
}

// Entity
tdapi string entity_name(EntityHandle entity) {
	return entity->lua_id;
}
tdapi int entity_id(EntityHandle entity) {
	return entity->id;
}
tdapi void on_collide(EntityHandle me, EntityHandle other) {
	def_get_cmp(cc, me, Collision_Component);
	if (cc)
		cc->on_collide(me, other);
}
tdapi void update_animation(EntityHandle me, float dt) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	auto anim = gc->active_animation;
	anim->time_to_next_frame -= dt;
	if (anim->time_to_next_frame <= 0.f) {
		anim->next_frame();
	}
}
tdapi int collider_kind(EntityHandle me) {
	def_get_cmp(cc, me.deref(), Collision_Component);
	return cc ?
		cc->kind :
		Collider_Kind::NO_COLLIDER;
}
tdapi void draw_entity(EntityHandle me, Render_Flags flags) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	def_get_cmp(pc, me.deref(), Position_Component);
	if (gc && pc) {
		renderer.draw(gc, pc, flags);
	}
}
tdapi void teleport_entity(EntityHandle me, float x, float y) {
	def_get_cmp(pc, me.deref(), Position_Component);
	if (pc)
		pc->world_pos = { x, y };
}
tdapi void move_entity(EntityHandle me, float dx, float dy) {
	def_get_cmp(mc, me.deref(), Movement_Component);
	mc->wish += glm::vec2(dx, dy);

	// Tell the Physics system to check + resolve if this movement causes collisions
	physics_system.movers.push_back(me);
}
tdapi EntityHandle create_entity(string lua_id) {
	return Entity::create(lua_id);
}
tdapi void set_animation(EntityHandle me, string wish_name) {
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
tdapi void set_animation_no_reset(EntityHandle me, string wish_name) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	if (gc->active_animation) {
		if (gc->active_animation->name == wish_name) {
			return;
		}
	}

	set_animation(me, wish_name);
}
tdapi void update_entity(EntityHandle me, float dt) {
	def_get_cmp(uc, me.deref(), Update_Component);
	if (uc)
		uc->update(me, dt);
}
tdapi bool are_interacting(EntityHandle initiator, EntityHandle receiver) {
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
tdapi void draw_aabb(EntityHandle me) {
	Points_Box box = get_bounding_box_world(me);
	draw_rect_world(box, green);
}

// Collision
tdapi bool are_entities_colliding(EntityHandle a, EntityHandle b) {
	optional<Center_Box> a_box = Center_Box::from_entity(a);
	optional<Center_Box> b_box = Center_Box::from_entity(b);
	glm::vec2 penetration;

	if (!a_box || !b_box) {
		return false;
	}

	return are_boxes_colliding(*a_box, *b_box, penetration);
}
tdapi void register_potential_collision(EntityHandle me, EntityHandle other) {
	// Check the collision matrix to see if these two collider kinds even need to test
	def_get_cmp(my_cc, me.deref(), Collision_Component);
	def_get_cmp(other_cc, me.deref(), Collision_Component);
	if (should_test_collision(my_cc->kind, other_cc->kind)) {
		physics_system.collisions.push_back(
			{ me, other }
		);
	}
}

// Input
tdapi bool is_down(GLFW_KEY_TYPE key) {
	return game.input.is_down[key];
}
tdapi bool was_down(GLFW_KEY_TYPE key) {
	return game.input.was_down[key];
}
tdapi bool was_pressed(GLFW_KEY_TYPE key) {
	return game.input.was_pressed(key);
}

// Game
tdapi void go_through_door(EntityHandle door, EntityHandle other) {
	EntityHandle hero = Lua.state["game"]["hero"];
	if (other == hero) {
		def_get_cmp(dc, door.deref(), Door_Component);
		Lua.state["game"]["level"] = levels[dc->to];
	}
}
tdapi void begin_dialogue(EntityHandle entity, string scene) {
	string npc = entity->lua_id;
	game.active_dialogue->init_from_table(npc, scene);
	Lua.state["game"]["state"] = Game_State::DIALOGUE;
}
tdapi void camera_follow(EntityHandle entity) {
	camera.following = entity;
}