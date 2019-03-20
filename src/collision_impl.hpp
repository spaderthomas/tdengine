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
				cout << "Collision found between " << element.me->name << ", " << element.other->name <<  endl;
				def_get_cmp(cc, element.me.deref(), Collision_Component);
				def_get_cmp(mypos, element.me.deref(), Position_Component);
				def_get_cmp(otherpos, element.other.deref(), Position_Component);
				mypos->world_pos -= penetration;
			}
		}
	}
	collisions.clear();
	movers.clear();
}

void collider_matrix_add(Collider_Kind me, Collider_Kind other, bool should_test) {
	collider_matrix[me][other] = should_test;
}
bool should_test_collision(Collider_Kind me, Collider_Kind other) {
	return collider_matrix[me][other];
}
void init_collider_matrix() {
	collider_matrix_add(Collider_Kind::DYNAMIC, Collider_Kind::STATIC, true);
	collider_matrix_add(Collider_Kind::DYNAMIC, Collider_Kind::DYNAMIC, true);
	collider_matrix_add(Collider_Kind::DYNAMIC, Collider_Kind::HERO, true);
	collider_matrix_add(Collider_Kind::DYNAMIC, Collider_Kind::DOOR, false);
	collider_matrix_add(Collider_Kind::DYNAMIC, Collider_Kind::NO_COLLIDER, false);

	collider_matrix_add(Collider_Kind::STATIC, Collider_Kind::STATIC, false);
	collider_matrix_add(Collider_Kind::STATIC, Collider_Kind::DYNAMIC, false);
	collider_matrix_add(Collider_Kind::STATIC, Collider_Kind::HERO, false);
	collider_matrix_add(Collider_Kind::STATIC, Collider_Kind::DOOR, false);
	collider_matrix_add(Collider_Kind::STATIC, Collider_Kind::NO_COLLIDER, false);

	collider_matrix_add(Collider_Kind::HERO, Collider_Kind::STATIC, true);
	collider_matrix_add(Collider_Kind::HERO, Collider_Kind::DYNAMIC, true);
	collider_matrix_add(Collider_Kind::HERO, Collider_Kind::HERO, dont_care);
	collider_matrix_add(Collider_Kind::HERO, Collider_Kind::DOOR, false);
	collider_matrix_add(Collider_Kind::HERO, Collider_Kind::NO_COLLIDER, false);

	collider_matrix_add(Collider_Kind::DOOR, Collider_Kind::STATIC, false);
	collider_matrix_add(Collider_Kind::DOOR, Collider_Kind::DYNAMIC, false);
	collider_matrix_add(Collider_Kind::DOOR, Collider_Kind::HERO, false);
	collider_matrix_add(Collider_Kind::DOOR, Collider_Kind::DOOR, false);
	collider_matrix_add(Collider_Kind::DOOR, Collider_Kind::NO_COLLIDER, false);

	collider_matrix_add(Collider_Kind::NO_COLLIDER, Collider_Kind::STATIC, false);
	collider_matrix_add(Collider_Kind::NO_COLLIDER, Collider_Kind::DYNAMIC, false);
	collider_matrix_add(Collider_Kind::NO_COLLIDER, Collider_Kind::HERO, false);
	collider_matrix_add(Collider_Kind::NO_COLLIDER, Collider_Kind::DOOR, false);
	collider_matrix_add(Collider_Kind::NO_COLLIDER, Collider_Kind::NO_COLLIDER, false);
}

Points_Box Center_Box::as_points() {
	float left = origin.x - extents.x / 2;
	float right = origin.x + extents.x / 2;
	float top = origin.y + extents.y / 2;
	float bottom = origin.y - extents.y / 2;

	return { top, bottom, left, right };
}
Center_Box Center_Box::from_points(Points_Box& points) {
	Center_Box box;
	box.origin = glm::vec2((points.left + points.right) / 2, (points.top + points.bottom) / 2);
	box.extents = glm::vec2(points.right - points.left, points.top - points.bottom);
	return box;
}
optional<Center_Box> Center_Box::from_entity(EntityHandle handle) {
	Entity* entity = handle();
	Center_Box box;
	def_get_cmp(pc, entity, Position_Component);
	def_get_cmp(cc, entity, Collision_Component);
	if (pc && cc) {
		box.origin = pc->world_pos + cc->bounding_box.screen_center;
		box.extents = cc->bounding_box.screen_extents;
		return box;
	}

	return {};
}

void Points_Box::convert_screen_to_gl() {
	left = gl_from_screen(left);
	right = gl_from_screen(right);
	top = gl_from_screen(top);
	bottom = gl_from_screen(bottom);
}
Center_Box Points_Box::as_center_box() {
	Center_Box center_box;
	center_box.origin = {
		(left + right) / 2.f,
		(top + bottom) / 2.f
	};
	center_box.extents = {
		(right - left),
		(top - bottom)
	};

	return center_box;
}

bool point_inside_box(glm::vec2& screen_pos, Center_Box& box) {
	Points_Box points = box.as_points();
	if (screen_pos.x > points.left && screen_pos.x < points.right
		&& screen_pos.y > points.bottom && screen_pos.y < points.top) {
		return true;
	}

	return false;
}