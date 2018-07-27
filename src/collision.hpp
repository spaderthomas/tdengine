bool are_boxes_colliding(Center_Box a, Center_Box b, glm::vec2& penetration) {
	// First, calculate the Minkowski difference
	Center_Box minkowski;
	minkowski.extents = a.extents + b.extents;
	float a_left = a.origin.x - .5f * a.extents.x;
	float b_right = b.origin.x + .5f * b.extents.x;
	minkowski.origin.x = a_left - b_right + .5f * minkowski.extents.x;
	float a_top = a.origin.y + .5f * a.extents.y;
	float b_bottom = b.origin.y - .5f * b.extents.y;
	minkowski.origin.y = a_top - b_bottom - .5f * minkowski.extents.y;

	Points_Box a_points = a.as_points();
	Points_Box b_points = b.as_points();
	glm::vec2 top_right = glm::vec2(a_points.right, a_points.top) - glm::vec2(b_points.left, b_points.bottom);
	glm::vec2 top_left = glm::vec2(a_points.left, a_points.top) - glm::vec2(b_points.right, b_points.bottom);
	glm::vec2 bottom_right = glm::vec2(a_points.right, a_points.bottom) - glm::vec2(b_points.left, b_points.top);
	glm::vec2 bottom_left = glm::vec2(a_points.left, a_points.bottom) - glm::vec2(b_points.right, b_points.top);
	Points_Box minkowski_;
	minkowski_.bottom = bottom_right.y;
	minkowski_.top = top_right.y;
	minkowski_.left = top_left.x;
	minkowski_.right = top_right.x;
	if (debug_show_minkowski) {
		draw_square_outline(minkowski_, red);
	}

	// If the Minkowski difference intersects the origin, there's a collision
	auto verts = minkowski.as_points();
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

struct {
	vector<Entity*> entities;

	void debug_draw_bounding_box(Entity* entity, glm::vec4 color) {
		auto box = Center_Box::from_entity(entity);
		if (box) {
			draw_square_outline((*box).as_points(), color);
		}
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
			
			if (!mc) { continue; } // Don't check if the other thing is static
			for (auto& other : entities) {
				if (other == entity) { continue; } // Don't check when you come across yourself
				auto entity_box = Center_Box::from_entity(entity);
				auto other_box = Center_Box::from_entity(other);
				if (entity_box && other_box) {
					(*entity_box).origin += mc->wish;
					glm::vec2 penetration;

					if (are_boxes_colliding(*entity_box, *other_box, penetration)) {
						mc->wish -= penetration;

						if (debug_show_aabb) {
							debug_draw_bounding_box(entity, blue);
							debug_draw_bounding_box(other, blue);
						}

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
