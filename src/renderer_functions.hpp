void Renderer::draw(Sprite* sprite, SRT transform) {
	Render_Element info = { sprite, transform };
	render_list.push_back(info);
}
void Renderer::render_for_frame() {
	bind_sprite_buffers();
	textured_shader.bind();
	textured_shader.set_int("sampler1", 0);
	textured_shader.set_vec3("camera_translation", camera_pos);
	float* matrix_buffer = (float*)calloc(9 * sizeof(render_list), sizeof(float));
	sort(render_list.begin(), render_list.end());

	for (auto& render_element : render_list) {
		Sprite* sprite = render_element.sprite;
		sprite->atlas->bind();

		sprite->bind();
 		auto transform_mat = mat3_from_transform(render_element.transform);
		textured_shader.set_mat3("transform", transform_mat);
		textured_shader.set_vec3("test", render_element.transform.translate);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	render_list.clear();
}
