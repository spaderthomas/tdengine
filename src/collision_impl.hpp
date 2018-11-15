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
bool are_boxes_colliding(Points_Box a, Points_Box b, glm::vec2& penetration) {
	//@hack just write this
	return are_boxes_colliding(a.as_center_box(), b.as_center_box(), penetration);
}

void Physics_System::process(float dt) {
	for (auto& mover : movers) {
		def_get_cmp(position, mover.deref(), Position_Component);
		def_get_cmp(movement, mover.deref(), Movement_Component);

		if (movement) {
			position->world_pos += movement->wish;
			movement->wish = glm::vec2(0.f);
		}
	}

	for (auto& element : collisions) {
		auto me_box = Center_Box::from_entity(element.me);
		auto other_box = Center_Box::from_entity(element.other);
		if (me_box && other_box) {
			glm::vec2 penetration;
			if (are_boxes_colliding(*me_box, *other_box, penetration)) {
				def_get_cmp(cc, element.me.deref(), Collision_Component);
				cc->on_collide(element.me, element.other);
			}
		}
	}
	collisions.clear();
	movers.clear();
}

// Lua exports
bool are_entities_colliding(EntityHandle a, EntityHandle b) {
	optional<Center_Box> a_box = Center_Box::from_entity(a);
	optional<Center_Box> b_box = Center_Box::from_entity(b);
	glm::vec2 penetration;

	if (!a_box || !b_box) {
		return false;
	}

	return are_boxes_colliding(*a_box, *b_box, penetration);
}
void register_potential_collision(EntityHandle me, EntityHandle other) {
	physics_system.collisions.push_back(
		{ me, other }
	);
}

