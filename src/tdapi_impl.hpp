
// Entity
tdapi Points_Box get_vision_box(EntityHandle me) {
	def_get_cmp(pc, me.deref(), Position_Component);
	def_get_cmp(vision, me.deref(), Vision_Component);

	Points_Box vision_box;
	vision_box.top = pc->world_pos.y + vision->depth;
	vision_box.bottom = pc->world_pos.y;
	vision_box.left = pc->world_pos.x - vision->width / 2.f;
	vision_box.right = pc->world_pos.x + vision->width / 2.f;

	return vision_box;
}
tdapi Points_Box get_bounding_box_world(EntityHandle me) {
	def_get_cmp(pc, me.deref(), Position_Component);
	def_get_cmp(cc, me.deref(), Collision_Component);
	fox_assert(cc);
	fox_assert(pc);

	Points_Box bounding_box;
	bounding_box.top = pc->world_pos.y + cc->bounding_box.screen_center.y + cc->bounding_box.screen_extents.y / 2;
	bounding_box.bottom = pc->world_pos.y + cc->bounding_box.screen_center.y - cc->bounding_box.screen_extents.y / 2;
	bounding_box.right = pc->world_pos.x + cc->bounding_box.screen_center.x + cc->bounding_box.screen_extents.x / 2;
	bounding_box.left = pc->world_pos.x + cc->bounding_box.screen_center.x - cc->bounding_box.screen_extents.x / 2;

	return bounding_box;
}
tdapi int collider_kind(EntityHandle me) {
	def_get_cmp(cc, me.deref(), Collision_Component);
	return cc ?
		cc->kind :
		Collider_Kind::NO_COLLIDER;
}
tdapi void draw_entity(EntityHandle me, Render_Flags flags) {
	if (!me) return;

	def_get_cmp(gc, me.deref(), Graphic_Component);
	def_get_cmp(pc, me.deref(), Position_Component);
	if (gc && pc) {
		renderer.draw(gc, pc, me, flags);
	}
}
tdapi void teleport_entity(EntityHandle me, float x, float y) {
	def_get_cmp(pc, me.deref(), Position_Component);
	if (pc)
		pc->world_pos = { x, y };
}
tdapi void move_entity(EntityHandle me, bool up, bool down, bool left, bool right) {
	def_get_cmp(mc, me.deref(), Movement_Component);
	if (up) mc->wish.y += mc->speed.y;
	if (down) mc->wish.y -= mc->speed.y;
	if (right) mc->wish.x += mc->speed.x;
	if (left) mc->wish.x -= mc->speed.x;

	// Tell the Physics system to check + resolve if this movement causes collisions
	physics_system.movers.push_back(me);
}
tdapi void set_animation(EntityHandle me, string wish_name) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	gc->begin_animation(wish_name);
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
tdapi void set_interaction(EntityHandle hero, EntityHandle other) {
	def_get_cmp(ic, other.deref(), Interaction_Component);
	if (ic) {
		ic->was_interacted_with = true;
		ic->other = hero;
	}
}
tdapi bool are_interacting(EntityHandle initiator, EntityHandle receiver) {
	// Check if the initiator's vision box collides with the receiver's bounding box
	def_get_cmp(cc, receiver.deref(), Collision_Component);
	def_get_cmp(receiver_pc, receiver.deref(), Position_Component);
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
		bool collision = are_boxes_colliding(get_vision_box(initiator), get_bounding_box_world(receiver), penetration);
		if (collision) {
			def_get_cmp(ic, receiver.deref(), Interaction_Component);
		}
		return collision;
	}
}

tdapi void draw_aabb(EntityHandle entity) {
	Points_Box box = get_bounding_box_world(entity);
	draw_rect_world(box, green);
}
tdapi void update_animation(EntityHandle entity, float dt) {
	def_get_cmp(gc, entity.deref(), Graphic_Component);
	auto anim = gc->active_animation;
	anim->time_to_next_frame -= dt;
	if (anim->time_to_next_frame <= 0.f) {
		anim->next_frame();
	}
}
tdapi void update_task(EntityHandle entity, float dt) {
	def_get_cmp(tc, entity.deref(), Task_Component);
	if (tc) {
		tc->task->update(dt);
	}
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
	def_get_cmp(other_cc, other.deref(), Collision_Component);
	if (should_test_collision(my_cc->kind, other_cc->kind)) {
		physics_system.collisions.push_back(
			{ me, other }
		);
	}
}


// State
tdapi void set_state(string var, bool val) {
	update_state(var, val);
}

