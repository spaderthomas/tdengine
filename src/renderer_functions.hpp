void Renderer::draw_sprite(Sprite* sprite, SRT transform) {
	Render_Info info = { sprite, transform };
	render_lists[sprite->atlas].push_back(info);
}

void Renderer::render_for_frame() {
	bind_sprite_buffers();
	textured_shader.bind();
	textured_shader.set_int("sampler1", 0);
	for (auto& render_list : render_lists) {
		Texture_Atlas* atlas = render_list.first;
		vector<Render_Info> sprites_to_render = render_list.second;

		atlas->bind();
		for (auto& to_render : sprites_to_render) {
			to_render.sprite->bind();
			auto transform_mat = mat3_from_transform(to_render.transform);
			textured_shader.set_mat3("transform", transform_mat);
			to_render.sprite->draw(GL_TRIANGLES);
		}

	}

	render_lists.clear();
}