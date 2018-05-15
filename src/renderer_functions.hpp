void Renderer::draw_sprite(Sprite* sprite, SRT transform, Render_Layer layer) {
	Render_Element info = { sprite, transform };
	if (layer == FOREGROUND) { fg_render_lists.push_back(info); }
	else if (layer == BACKGROUND) { bg_render_lists.push_back(info); }
	else {
		string msg = "Tried to draw sprite: " + sprite->name + ", but no layer was set.";
		tdns_log.write(msg.c_str());
	}
}
void Renderer::draw_sprite(Sprite* sprite, glm::ivec2 grid_pos, glm::vec2 scale, Render_Layer layer) {
	SRT transform = srt_from_grid_pos(grid_pos);
	transform.scale - scale;
	Render_Element info = { sprite, transform };
	if (layer == FOREGROUND) { fg_render_lists.push_back(info); }
	else if (layer == BACKGROUND) { bg_render_lists.push_back(info); }
	else {
		string msg = "Tried to draw sprite: " + sprite->name + ", but no layer was set.";
		tdns_log.write(msg.c_str());
	}
}

void Renderer::render_for_frame() {
	bind_sprite_buffers();
	textured_shader.bind();
	textured_shader.set_int("sampler1", 0);
	textured_shader.set_vec3("camera_translation", camera_pos);
	vector<Render_Element>* layers_lists[2] = { &bg_render_lists, &fg_render_lists };
	fox_for(ilayer, NUM_LAYERS) {
		auto& sprites_to_render = *layers_lists[ilayer];
		sort(sprites_to_render.begin(), sprites_to_render.end());

		for (auto& render_element : sprites_to_render) {
			Sprite* sprite = render_element.sprite;
			if (sprite->atlas->handle != last_bound_texture) {
				sprite->atlas->bind();
			}

			sprite->bind((Render_Layer)ilayer);
			auto transform_mat = mat3_from_transform(render_element.transform);
			textured_shader.set_mat3("transform", transform_mat);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, sprite->indices_offset);
		}
	}

	fox_for(ilayer, NUM_LAYERS) {
		auto& all_atlas_render_list = *layers_lists[ilayer];
		all_atlas_render_list.clear();
	}
}
