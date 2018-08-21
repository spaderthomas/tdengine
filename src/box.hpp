struct Points_Box {
	screen_unit top;
	screen_unit bottom;
	screen_unit left;
	screen_unit right;
};

void convert_screen_to_gl(Points_Box& points) {
	points.left = gl_from_screen(points.left);
	points.right = gl_from_screen(points.right);
	points.top = gl_from_screen(points.top);
	points.bottom = gl_from_screen(points.bottom);
}

struct Center_Box {
	glm::vec2 origin;
	glm::vec2 extents;

	Points_Box as_points() {
		screen_unit left = origin.x - extents.x / 2;
		screen_unit right = origin.x + extents.x / 2;
		screen_unit top = origin.y + extents.y / 2;
		screen_unit bottom = origin.y - extents.y / 2;

		return { top, bottom, left, right };
	}

	static optional<Center_Box> from_entity(pool_handle<Entity> handle) {
		Entity* entity = entity_pool.get(handle);
		Center_Box box;
		Position_Component* pc = entity->get_component<Position_Component>();
		Bounding_Box* cc = entity->get_component<Bounding_Box>();
		if (pc && cc) {
			box.origin = pc->screen_pos + cc->screen_center;
			box.extents = cc->screen_extents;
			return box;
		}

		return {};
	}

	static Center_Box from_points(Points_Box& points) {
		Center_Box box;
		box.origin = glm::vec2((points.left + points.right) / 2, (points.top + points.bottom) / 2);
		box.extents = glm::vec2(points.right - points.left, points.top - points.bottom);
		return box;
	}
};


bool point_inside_box(glm::vec2& screen_pos, Center_Box& box) {
	Points_Box points = box.as_points();
	if (screen_pos.x > points.left && screen_pos.x < points.right
		&& screen_pos.y > points.bottom && screen_pos.y < points.top) {
		return true;
	}

	return false;
}
