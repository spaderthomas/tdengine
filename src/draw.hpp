void draw_line_from_origin(glm::vec2 basis, glm::vec4 color) {
	solid_shader.bind();
	solid_shader.set_vec4("color", color);
	SRT transform = SRT::no_transform();
	transform.scale = basis;
	auto transform_mat = mat3_from_transform(transform);
	solid_shader.set_mat3("transform", transform_mat);
	line->bind();
	line->draw(GL_LINES);
}
void draw_line_from_points(glm::vec2 p1, glm::vec2 p2, glm::vec4 color) {
	solid_shader.bind();
	solid_shader.set_vec4("color", color);
	SRT transform = SRT::no_transform();
	transform.scale = p2 - p1;
	transform.translate = glm::vec3(p1, 0.f);
	auto transform_mat = mat3_from_transform(transform);
	solid_shader.set_mat3("transform", transform_mat);
	line->bind();
	line->draw(GL_LINES);
}

void draw_square(SRT transform, glm::vec4 color) {
	auto trans_mat = mat3_from_transform(transform);
	solid_shader.bind();
	solid_shader.set_mat3("transform", trans_mat);
	solid_shader.set_vec4("color", color);
	square->bind();
	square->draw(GL_TRIANGLES);
}
