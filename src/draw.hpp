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
	transform.translate = p1;
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
void draw_tile(SRT transform, Texture* texture) {
	auto trans_mat = mat3_from_transform(transform);
	textured_shader.bind();
	textured_shader.set_mat3("transform", trans_mat);
	texture->bind();
	textured_shader.set_int("sampler1", 0);
	square->bind();
	square->draw(GL_TRIANGLES);
}
