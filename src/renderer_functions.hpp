void Renderer::draw_sprite(Sprite* sprite, SRT transform, Render_Layer layer) {
	Render_Element info = { sprite, transform };
	if (layer == FOREGROUND) { fg_render_lists[sprite->atlas].push_back(info); }
	else if (layer == BACKGROUND) { bg_render_lists[sprite->atlas].push_back(info); }
}
void Renderer::draw_sprite(Sprite* sprite, glm::ivec2 grid_pos, glm::vec2 scale, Render_Layer layer) {
	SRT transform = srt_from_grid_pos(grid_pos);
	transform.scale - scale;
	Render_Element info = { sprite, transform };
	if (layer == FOREGROUND) { fg_render_lists[sprite->atlas].push_back(info); }
	else if (layer == BACKGROUND) { bg_render_lists[sprite->atlas].push_back(info); }
}

void Renderer::render_for_frame() {
	bind_sprite_buffers();
	textured_shader.bind();
	textured_shader.set_int("sampler1", 0);
	unordered_map<Texture_Atlas*, vector<Render_Element>>* layers_lists[2] = { &bg_render_lists, &fg_render_lists };
	fox_for(ilayer, NUM_LAYERS) {
		auto& all_atlas_render_list = *layers_lists[ilayer];
		for(auto& atlas_render_list : all_atlas_render_list) {
			Texture_Atlas* atlas = atlas_render_list.first;
			vector<Render_Element> sprites_to_render = atlas_render_list.second;
			sort(sprites_to_render.begin(), sprites_to_render.end());

			atlas->bind();
			for (auto& to_render : sprites_to_render) {
				to_render.sprite->bind((Render_Layer)ilayer);
				auto transform_mat = mat3_from_transform(to_render.transform);
				textured_shader.set_mat3("transform", transform_mat);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, to_render.sprite->indices_offset);
			}
		}
	}

	fox_for(ilayer, NUM_LAYERS) {
		auto& all_atlas_render_list = *layers_lists[ilayer];
		all_atlas_render_list.clear();
	}
}