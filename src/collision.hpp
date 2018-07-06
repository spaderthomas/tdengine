struct Rectangle_Points {
	screen_unit left;
	screen_unit right;
	screen_unit top;
	screen_unit bottom;
};

struct {
	vector<Entity*> entities;

	// An internal struct for combining Bounding_Box (which is relative to the center of the thing it bounds) and actual positions
	struct Absolute_Bounding_Box {
		glm::vec2 origin;
		glm::vec2 extents;

		Rectangle_Points get_verts() {
			screen_unit left = origin.x - extents.x / 2;
			screen_unit right = origin.x + extents.x / 2;
			screen_unit top = origin.y + extents.y / 2;
			screen_unit bottom = origin.y - extents.y / 2;

			return { left, right, top, bottom };
		}

		static Absolute_Bounding_Box from_entity(Entity* e) {
			Absolute_Bounding_Box box;
			Position_Component* pc = e->get_component<Position_Component>();
			Bounding_Box* cc = e->get_component<Bounding_Box>();
			if (pc && cc) {
				box.origin = pc->screen_pos + cc->screen_center;
				box.extents = cc->screen_extents;
			} else {
				tdns_log.write("Tried to make an absolute bounding box, but didn't have either position or bounding box");
				tdns_log.write("Lua ID: " + e->lua_id);
			}

			return box;
		}
	};

	void debug_draw_bounding_box(Entity* entity, glm::vec4 color) {
		Absolute_Bounding_Box box = Absolute_Bounding_Box::from_entity(entity);
		Rectangle_Points verts = box.get_verts();

		gl_unit gl_left = gl_from_screen(verts.left);
		gl_unit gl_right = gl_from_screen(verts.right);
		gl_unit gl_top = gl_from_screen(verts.top);
		gl_unit gl_bottom = gl_from_screen(verts.bottom);

		draw_square_outline(glm::vec2(gl_left, gl_top), glm::vec2(gl_right, gl_top), glm::vec2(gl_right, gl_bottom), glm::vec2(gl_left, gl_bottom), color);
	}

	bool are_entities_colliding(Entity* a, Entity* b) {
		return are_boxes_colliding(Absolute_Bounding_Box::from_entity(a), Absolute_Bounding_Box::from_entity(b));
	}

	bool are_boxes_colliding(Absolute_Bounding_Box a, Absolute_Bounding_Box b) {
		Rectangle_Points verts_a = a.get_verts();
		Rectangle_Points verts_b = b.get_verts();

		if (verts_a.left < verts_b.right && verts_a.right > verts_b.left && 
			verts_a.bottom < verts_b.top && verts_a.top > verts_b.bottom) {
			return true;
		}

		return false;
	}

	void process(float dt) {
		// Render collision boxes
		for (auto ent : entities) {
			debug_draw_bounding_box(ent, white4);
		}

		// First, figure out which entities are trying to move.
		for (auto& entity : entities) {
			auto mc = entity->get_component<Movement_Component>();
			auto pc = entity->get_component<Position_Component>();
			
			if (!mc) { continue; }
			for (auto& other : entities) {
				if (other == entity) { continue; } // Don't check when you come across yourself
				Absolute_Bounding_Box entity_box = Absolute_Bounding_Box::from_entity(entity);
				Absolute_Bounding_Box other_box = Absolute_Bounding_Box::from_entity(other);
				entity_box.origin += mc->wish;
				if (are_boxes_colliding(entity_box, other_box)) {
					mc->wish = glm::vec2(0.f);
					debug_draw_bounding_box(entity, blue);
					debug_draw_bounding_box(other, blue);
				}
			}
			
			// Collision and movement handled; clear out the desired motion for this frame
			pc->screen_pos += mc->wish;
			mc->wish = glm::vec2(0.f);
		}

		entities.clear();
	}
} physics_system;

#if 0
	Position_Component* pc;
	Collision_Component* cc;
	Movement_Component* mc;
	fox_for(ient, entities.size()) {
		auto ent = entities[ient];
		pc = ent->get_component<Position_Component>();
		cc = ent->get_component<Collision_Component>();
		mc = ent->get_component<Movement_Component>();
		// From the center of the entity (i.e. its PC), add the offset to the center of the bounding box, then half the extent (since we're already at the center), then adjust by the camera 
		screen_unit left = pc->screen_pos.x + cc->screen_center.x - cc->screen_extents.x / 2 - camera_top_left.x;
		screen_unit right = pc->screen_pos.x + cc->screen_center.x + cc->screen_extents.x / 2 - camera_top_left.x;
		screen_unit top = pc->screen_pos.y + cc->screen_center.y + cc->screen_extents.y / 2 - camera_top_left.y;
		screen_unit bottom = pc->screen_pos.y + cc->screen_center.y - cc->screen_extents.y / 2 - camera_top_left.y;
		gl_unit gl_left = gl_from_screen(left);
		gl_unit gl_right = gl_from_screen(right);
		gl_unit gl_top = gl_from_screen(top);
		gl_unit gl_bottom = gl_from_screen(bottom);

		Position_Component* pc2;
		Collision_Component* cc2;
		Movement_Component* mc2;
		for (int ient2 = ient + 1; ient2 < entities.size(); ient2++) {
			auto ent2 = entities[ient2];
			pc2 = ent2->get_component<Position_Component>();
			cc2 = ent2->get_component<Collision_Component>();
			mc2 = ent2->get_component<Movement_Component>();
			screen_unit left2 = pc2->screen_pos.x + cc2->screen_center.x - cc2->screen_extents.x / 2 - camera_top_left.x;
			screen_unit right2 = pc2->screen_pos.x + cc2->screen_center.x + cc2->screen_extents.x / 2 - camera_top_left.x;
			screen_unit top2 = pc2->screen_pos.y + cc2->screen_center.y + cc2->screen_extents.y / 2 - camera_top_left.y;
			screen_unit bottom2 = pc2->screen_pos.y + cc2->screen_center.y - cc2->screen_extents.y / 2 - camera_top_left.y;
			gl_unit gl_left2 = gl_from_screen(left2);
			gl_unit gl_right2 = gl_from_screen(right2);
			gl_unit gl_top2 = gl_from_screen(top2);
			gl_unit gl_bottom2 = gl_from_screen(bottom2);

			auto is_between = [](float upper_bound, float lower_bound, float input) -> bool {
				return (input > lower_bound) && (input < upper_bound);
			};
			if ((is_between(top, bottom, top2) || is_between(top, bottom, bottom2)) &&
				(is_between(right, left, left2) || is_between(right, left, right2))) {
				if (mc) {
					draw_square_outline(glm::vec2(gl_left, gl_top), glm::vec2(gl_right, gl_top), glm::vec2(gl_right, gl_bottom), glm::vec2(gl_left, gl_bottom), blue);
				}
				if (mc2) {
					draw_square_outline(glm::vec2(gl_left2, gl_top2), glm::vec2(gl_right2, gl_top2), glm::vec2(gl_right2, gl_bottom2), glm::vec2(gl_left2, gl_bottom2), blue);
				}
			}
		}
	}
#endif