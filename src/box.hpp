struct Points_Box;
struct Center_Box {
	glm::vec2 origin;
	glm::vec2 extents;

	static optional<Center_Box> from_entity(EntityHandle handle);
	static Center_Box from_points(Points_Box& points);
	Points_Box as_points();
};

struct Points_Box {
	float top;
	float bottom;
	float left;
	float right;

	void convert_screen_to_gl();
	Center_Box as_center_box();
};


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