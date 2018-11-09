/* 
How to use this little shape drawing API:
Pass any arguments to draw functions in screen coordinates. That function will translate them into GL coordinates and make the draw call.
Keep it this way to separate concerns: The game (or whatever system) doesn't need to know about GL coordinate system!
*/



void draw_line_from_origin(glm::vec2 basis, glm::vec4 color) {
	basis = gl_from_screen(basis);
	auto draw = [basis, color]() -> void {
		solid_shader.begin();
		glm::vec4 color_ = color;
		solid_shader.set_vec4("color", color_);
		SRT transform = SRT::no_transform();
		transform.scale = basis;
		auto transform_mat = mat3_from_transform(transform);
		solid_shader.set_mat3("transform", transform_mat);
		line->bind();
		solid_shader.check();
		line->draw(GL_LINES);
		solid_shader.end();
	};
	renderer.primitives.push_back(draw);
}
void draw_line_from_points(glm::vec2 p1, glm::vec2 p2, glm::vec4 color) {
	p1 = gl_from_screen(p1);
	p2 = gl_from_screen(p2);
	auto draw = [p1, p2, color]() -> void {
		solid_shader.begin();
		glm::vec4 color_ = color; //@hack So I can pass as a reference everywhere else. maybe make it a pointer? but then inconsistent :(
		solid_shader.set_vec4("color", color_);

		SRT transform = SRT::no_transform();
		//glm::vec2 camera_translation = magnitude_gl_from_screen(glm::vec2(.5, .5) - _camera.offset);
 		transform.scale = p2 - p1;
		transform.translate = glm::vec3(p1, 1.f);
		//transform.translate += camera_translation;
		auto transform_mat = mat3_from_transform(transform);
		solid_shader.set_mat3("transform", transform_mat);

		line->bind();
		solid_shader.check();
		line->draw(GL_LINES);

		solid_shader.end();
	};
	renderer.primitives.push_back(draw);

}


void draw_square_outline(glm::vec2 top_left, glm::vec2 top_right, glm::vec2 bottom_right, glm::vec2 bottom_left, glm::vec4 color) {
	draw_line_from_points(top_left, top_right, color);
	draw_line_from_points(top_right, bottom_right, color);
	draw_line_from_points(bottom_right, bottom_left, color);
	draw_line_from_points(bottom_left, top_left, color);
}
void draw_square_outline(screen_unit top, screen_unit bottom, screen_unit left, screen_unit right, glm::vec4 color) {
	glm::vec2 top_left = glm::vec2(left, top);
	glm::vec2 top_right = glm::vec2(right, top);
	glm::vec2 bottom_right = glm::vec2(right, bottom);
	glm::vec2 bottom_left = glm::vec2(left, bottom);
	draw_square_outline(top_left, top_right, bottom_right, bottom_left, color);
}
void draw_square_outline(Points_Box points, glm::vec4 color) {
	glm::vec2 top_left = glm::vec2(points.left, points.top);
	glm::vec2 top_right = glm::vec2(points.right, points.top);
	glm::vec2 bottom_right = glm::vec2(points.right, points.bottom);
	glm::vec2 bottom_left = glm::vec2(points.left, points.bottom);
	draw_square_outline(top_left, top_right, bottom_right, bottom_left, color);
}
void draw_square_outline(Points_Box points, SRT transform, glm::vec4 color) {
	auto mat = mat3_from_transform(transform);
	glm::vec3 top_left = mat * glm::vec3(points.left, points.top, 1.f);
	glm::vec3 top_right = mat * glm::vec3(points.right, points.top, 1.f);
	glm::vec3 bottom_right = mat * glm::vec3(points.right, points.bottom, 1.f);
	glm::vec3 bottom_left = mat * glm::vec3(points.left, points.bottom, 1.f);
	draw_square_outline(top_left, top_right, bottom_right, bottom_left, color);
}


void draw_square(Center_Box box, glm::vec4 color) {
	auto draw = [box, color]() -> void {
		auto transform = SRT::no_transform();
		transform.translate = gl_from_screen(box.origin);
		transform.scale = box.extents;
		auto trans_mat = mat3_from_transform(transform);
		solid_shader.begin();
		solid_shader.set_mat3("transform", trans_mat);
		solid_shader.set_vec4("color", color);
		square->bind();
		solid_shader.check();
		square->draw(GL_TRIANGLES);
		solid_shader.end();
	};
	renderer.primitives.push_back(draw);
}

void debug_draw_bounding_box(EntityHandle handle, glm::vec4 color) {
	auto box = Center_Box::from_entity(handle);
	if (box) {
		draw_square_outline((*box).as_points(), color);
	}
}