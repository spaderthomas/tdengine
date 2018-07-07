vector<function<void()>> render_on_top;

void draw_line_from_origin(glm::vec2 basis, glm::vec4 color) {
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
	render_on_top.push_back(draw);
}
void draw_line_from_points(glm::vec2 p1, glm::vec2 p2, glm::vec4 color) {
	auto draw = [p1, p2, color]() -> void {
		solid_shader.begin();
		glm::vec4 color_ = color; //@hack So I can pass as a reference everywhere else. maybe make it a pointer? but then inconsistent :(
		solid_shader.set_vec4("color", color_);
		SRT transform = SRT::no_transform();
		transform.scale = p2 - p1;
		transform.translate = glm::vec3(p1, 1.f);
		auto transform_mat = mat3_from_transform(transform);
		solid_shader.set_mat3("transform", transform_mat);
		line->bind();
		solid_shader.check();
		line->draw(GL_LINES);

		solid_shader.end();
	};
	render_on_top.push_back(draw);

}

void draw_square_outline(glm::vec2 top_left, glm::vec2 top_right, glm::vec2 bottom_right, glm::vec2 bottom_left, glm::vec4 color) {
	draw_line_from_points(top_left, top_right, color);
	draw_line_from_points(top_right, bottom_right, color);
	draw_line_from_points(bottom_right, bottom_left, color);
	draw_line_from_points(bottom_left, top_left, color);
}
void draw_square_outline(SRT transform, glm::vec4 color) {
	glm::vec3 top_left = mat3_from_transform(transform) * screen_top_left;
	glm::vec3 top_right = mat3_from_transform(transform) * screen_top_right;
	glm::vec3 bottom_right = mat3_from_transform(transform) * screen_bottom_right;
	glm::vec3 bottom_left = mat3_from_transform(transform) * screen_bottom_left;
	draw_square_outline(top_left, top_right, bottom_right, bottom_left, color);
}

void draw_square(SRT transform, glm::vec4 color) {
	auto draw = [&transform, &color]() -> void {
		auto trans_mat = mat3_from_transform(transform);
		solid_shader.begin();
		solid_shader.set_mat3("transform", trans_mat);
		solid_shader.set_vec4("color", color);
		square->bind();
		solid_shader.check();
		square->draw(GL_TRIANGLES);
		solid_shader.end();
	};
	render_on_top.push_back(draw);

}
void draw_rectangle(glm::vec2 bottom_left, glm::vec2 extents, glm::vec4 color) {
	SRT transform = SRT::no_transform();
	transform.scale = extents;
	transform.translate = glm::vec3(bottom_left.x + extents.x, bottom_left.y + extents.y, 1.f);
	draw_square(transform, color);
}
