struct Rectangle_Points {
	screen_unit left;
	screen_unit right;
	screen_unit top;
	screen_unit bottom;
};

// A struct for combining Bounding_Box (which is relative to the center of the thing it bounds) and actual positions
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
		fox_assert(pc);
		fox_assert(cc);
		box.origin = pc->screen_pos + cc->screen_center;
		box.extents = cc->screen_extents;

		return box;
	}

	void debug_draw(glm::vec4 color) {
		Rectangle_Points verts = get_verts();

		gl_unit gl_left = gl_from_screen(verts.left);
		gl_unit gl_right = gl_from_screen(verts.right);
		gl_unit gl_top = gl_from_screen(verts.top);
		gl_unit gl_bottom = gl_from_screen(verts.bottom);

		draw_square_outline(glm::vec2(gl_left, gl_top), glm::vec2(gl_right, gl_top), glm::vec2(gl_right, gl_bottom), glm::vec2(gl_left, gl_bottom), color);
	}
};

bool point_inside_box(glm::vec2& screen_pos, Absolute_Bounding_Box& box) {
	Rectangle_Points points = box.get_verts();
	if (screen_pos.x > points.left && screen_pos.x < points.right
		&& screen_pos.y > points.bottom && screen_pos.y < points.top) {
		return true;
	}

	return false;
}

struct {
	vector<Entity*> entities;

	
	void debug_draw_bounding_box(Entity* entity, glm::vec4 color) {
		Absolute_Bounding_Box box = Absolute_Bounding_Box::from_entity(entity);
		box.debug_draw(color);
	}

	bool are_boxes_colliding(Absolute_Bounding_Box a, Absolute_Bounding_Box b, glm::vec2& penetration) {
		// First, calculate the Minkowski difference
		Absolute_Bounding_Box minkowski;
		minkowski.extents = a.extents + b.extents;
		float a_left = a.origin.x - .5f * a.extents.x;
		float b_right = b.origin.x + .5f * b.extents.x;
		minkowski.origin.x = a_left - b_right + .5f * minkowski.extents.x;
		float a_top = a.origin.y + .5f * a.extents.y;
		float b_bottom = b.origin.y - .5f * b.extents.y;
		minkowski.origin.y = a_top - b_bottom - .5f * minkowski.extents.y;
	
		if (debug_show_aabb) { minkowski.debug_draw(red); }

		// If the Minkowski difference intersects the origin, there's a collision
		auto verts = minkowski.get_verts();
		if (verts.right >= 0 && verts.left <= 0 && verts.top >= 0 && verts.bottom <= 0) {
			// The pen vector is the shortest vector from the origin of the MD to an edge.
			// You know this has to be a vertical or horizontal line from the origin (these are by def. the shortest)
			float min = 100000.f;
			if (abs(verts.left) < min) {
				min = abs(verts.left);
				penetration = glm::vec2(verts.left, 0.f);
			}
			if (abs(verts.right) < min) {
				min = abs(verts.right);
				penetration = glm::vec2(verts.right, 0.f);
			}
			if (abs(verts.top) < min) {
				min = abs(verts.top);
				penetration = glm::vec2(0.f, verts.top);
			}
			if (abs(verts.bottom) < min) {
				min = abs(verts.bottom);
				penetration = glm::vec2(0.f, verts.bottom);
			}

			return true;
		}

		penetration = glm::vec2(0.f);
		return false;
	}

	void process(float dt) {
		// Render collision boxes
		if (debug_show_aabb) {
			for (auto ent : entities) {
				debug_draw_bounding_box(ent, white4);

			}
		}

		// First, figure out which entities are trying to move.
		for (auto& entity : entities) {
			auto mc = entity->get_component<Movement_Component>();
			auto pc = entity->get_component<Position_Component>();
			
			if (!mc) { continue; }
			for (auto& other : entities) {
				if (other == entity) { continue; } // Don't check when you come across yourself
				Absolute_Bounding_Box entity_box = Absolute_Bounding_Box::from_entity(entity);
				entity_box.origin += mc->wish;
				Absolute_Bounding_Box other_box = Absolute_Bounding_Box::from_entity(other);
				glm::vec2 penetration;

				if (are_boxes_colliding(entity_box, other_box, penetration)) {
					mc->wish -= penetration;
					
					if (debug_show_aabb) {
						debug_draw_bounding_box(entity, blue);
						debug_draw_bounding_box(other, blue);
					}
					
				}
			}
			
			// Collision and movement handled; clear out the desired motion for this frame
			pc->screen_pos += mc->wish;
			mc->wish = glm::vec2(0.f);
		}

		entities.clear();
	}
} physics_system;
