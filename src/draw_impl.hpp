void init_gl() {
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallbackKHR(gl_debug_callback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	glGenVertexArrays(1, &Sprite::vao);
	glGenBuffers(1, &Sprite::vert_buffer);
	glGenBuffers(1, &Sprite::elem_buffer);
	glGenVertexArrays(1, &Mesh::vao);
	glGenBuffers(1, &Mesh::vert_buffer);
	glGenBuffers(1, &Mesh::elem_buffer);
	
	
	// Fill GPU sprite buffers
	glBindVertexArray(Sprite::vao);
	std::vector<float> vert_data;
	
	concat(vert_data, square_verts);
	
	// Fill tex coordinate buffer
	auto& asset_manager = get_asset_manager();
	auto sprites = asset_manager.get_all<Sprite>();
	for (auto sprite : sprites) {
		sprite->tex_coord_offset = (GLvoid*)(sizeof(float) * vert_data.size());
		concat(vert_data, sprite->tex_coords);
	}
	
	square_tex_coords_offset = (GLvoid*)(sizeof(float) * vert_data.size());
	concat(vert_data, square_tex_coords);
	
	// Send all the data to OpenGL buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, square_indices.size() * sizeof(uint), square_indices.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_data.size() * sizeof(float), vert_data.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	
	
	// Fill GPU mesh buffers
	glBindVertexArray(Mesh::vao);
	std::vector<Mesh*> all_meshes = asset_manager.get_all<Mesh>();
	std::vector<float> vert_buffer;
	std::vector<uint> indx_buffer;
	
	// Collect all vertices and indices
	fox_for(imesh, all_meshes.size()) {
		Mesh* mesh = all_meshes[imesh];
		mesh->indices_offset = (GLvoid*)(sizeof(uint) * indx_buffer.size());
		indx_buffer.insert(indx_buffer.end(), mesh->indices.begin(), mesh->indices.end());
		mesh->vert_offset = (GLvoid*)(sizeof(float) * vert_buffer.size());
		vert_buffer.insert(vert_buffer.end(), mesh->verts.begin(), mesh->verts.end());
	}
	// Collect all tex coords
	fox_for(imesh, all_meshes.size()) {
		Mesh* mesh = all_meshes[imesh];
		mesh->tex_coord_offset = (GLvoid*)(sizeof(float) * vert_buffer.size());
		vert_buffer.insert(vert_buffer.end(), mesh->tex_coords.begin(), mesh->tex_coords.end());
	}
	
	// Give them to the OpenGL buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indx_buffer.size() * sizeof(uint), indx_buffer.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, Mesh::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_buffer.size() * sizeof(float), vert_buffer.data(), GL_STATIC_DRAW);
}
	
void draw_line_from_points(glm::vec2 p1, glm::vec2 p2, glm::vec4 color) {
	p1 = gl_from_screen(p1);
	p2 = gl_from_screen(p2);
	auto draw = [p1, p2, color]() -> void {
		auto& shaders = get_shader_manager();
		auto& solid_shader = *shaders.get("solid");

		solid_shader.begin();
		solid_shader.set_vec4("color", color);

		SRT transform = SRT::no_transform();
		transform.scale = p2 - p1;
		transform.translate = glm::vec3(p1, 1.f);
		solid_shader.set_mat3("transform", mat3_from_transform(transform));

		auto& asset_manager = get_asset_manager();
		Mesh* line = asset_manager.get_asset<Mesh>("line");
		line->bind();
		solid_shader.check();
		line->draw(GL_LINES);

		solid_shader.end();
	};
	
	auto& render_engine = get_render_engine();
	render_engine.primitives.push_back(draw);
}
void draw_line_from_origin(glm::vec2 basis, glm::vec4 color) {
	basis = gl_from_screen(basis);
	auto draw = [basis, color]() -> void {
		auto& shaders = get_shader_manager();
		auto& solid_shader = *shaders.get("solid");

		solid_shader.begin();
		solid_shader.set_vec4("color", color);
		SRT transform = SRT::no_transform();
		transform.scale = basis;
		auto transform_mat = mat3_from_transform(transform);
		solid_shader.set_mat3("transform", transform_mat);

		auto& asset_manager = get_asset_manager();
		Mesh* line = asset_manager.get_asset<Mesh>("line");
		line->bind();
		solid_shader.check();
		line->draw(GL_LINES);
		solid_shader.end();
	};
	auto& render_engine = get_render_engine();
	render_engine.primitives.push_back(draw);
}

void draw_rect_filled_screen(glm::vec2 origin, glm::vec2 extents, glm::vec4 color) {
	auto draw = [origin, extents, color]() -> void {
		auto& shaders = get_shader_manager();
		auto& solid_shader = *shaders.get("solid");

		solid_shader.begin();

		auto transform = SRT::no_transform();
		transform.translate = gl_from_screen(origin);
		transform.scale = extents;
		auto trans_mat = mat3_from_transform(transform);
		solid_shader.set_mat3("transform", trans_mat);
		
		solid_shader.set_vec4("color", color);

		auto& asset_manager = get_asset_manager();
		Mesh* square = asset_manager.get_asset<Mesh>("square");

		square->bind();
		solid_shader.check();
		square->draw(GL_TRIANGLES);
		solid_shader.end();
	};

	auto& render_engine = get_render_engine();
	render_engine.primitives.push_back(draw);
}

void draw_rect_outline_screen(glm::vec2 tl, glm::vec2 tr, glm::vec2 br, glm::vec2 bl, glm::vec4 color) {
	draw_line_from_points(tl, tr, color);
	draw_line_from_points(tr, br, color);
	draw_line_from_points(br, bl, color);
	draw_line_from_points(bl, tl, color);
}

void draw_rect_outline_screen(glm::vec2 origin, glm::vec2 extents, glm::vec4 color) {
	// Extents is the length from side to side, so the distance from the origin to the extreme uses half the extent
	extents.x = extents.x / 2;
	extents.y = extents.y / 2;
	glm::vec2 bl = {origin.x - extents.x, origin.y - extents.y};
	glm::vec2 tl = {origin.x - extents.x, origin.y + extents.y};
	glm::vec2 br = {origin.x + extents.x, origin.y - extents.y};
	glm::vec2 tr = {origin.x + extents.x, origin.y + extents.y};
	
	draw_rect_outline_screen(tl, tr, br, bl, color);
}

void draw_rect_outline_world(glm::vec2 origin, glm::vec2 extents, glm::vec4 color) {
	auto& render_engine = get_render_engine();

	glm::vec2 camera_translation{render_engine.camera.x, render_engine.camera.y};

	origin += camera_translation;
	
	draw_rect_outline_screen(origin, extents, color);	
}

void draw_text(std::string text, glm::vec2 point, Text_Flags flags) {
	TextRenderInfo info;
	info.text = text;
	info.point = point;
	info.flags = flags;

	auto& render_engine = get_render_engine();
	render_engine.text_infos.push_back(info);
}

RenderEngine& get_render_engine() {
	static RenderEngine engine;
	return engine;
}

void RenderEngine::init() {
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	// Generate the color buffer, allocate GPU memory for it, and attach it to the frame buffer
	glGenTextures(1, &color_buffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_buffer);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2560, 1440, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);
	
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		tdns_log.write("@incomplete_frame_buffer: " + std::to_string(status));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Camera& RenderEngine::get_camera() {
	return camera;
}

void RenderEngine::remove_entity(int entity) {
	for (auto it = render_list.begin(); it != render_list.end();) {
		if (it->entity == entity) {
			it = render_list.erase(it);
		} else {
			it++;
		}
	}
}

void RenderEngine::render(float dt) {
	if (camera.snap_to_player || camera.follow_player) {
		auto& physics = get_physics_engine();
		auto& entity_manager = get_entity_manager();
		
		auto position = physics.get_position(entity_manager.player);
		camera.x = -1 * position->x + .5;
		camera.y = -1 * position->y + .5;

		camera.snap_to_player = false;
	}
	
	render_scene(dt);
	render_text(dt);
	render_primitives(dt);
}

void RenderEngine::render_scene(float dt) {
	// If we're applying post processing, make sure to render to a frame buffer
	if (post_processing_info.type != PostProcessingType::None) {
		glViewport(0, 0, 2560, 1440);
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	bind_sprite_buffers();

	auto& shaders = get_shader_manager();
	Shader* shader = shaders.get("textured");
	Shader* highlighted_shader = shaders.get("highlighted");
	Shader* textured_shader = shaders.get("textured");

	shader->begin();

	shader->set_int("sampler", 0);

	auto sort_render_list = [](const auto& ra, const auto& rb) {
		if (ra.layer != rb.layer) return ra.layer < rb.layer;
		auto& physics_engine = get_physics_engine();
		auto pa = physics_engine.get_position(ra.entity);
		auto pb = physics_engine.get_position(rb.entity);
		return pa->y > pb->y; 
	};
	stable_sort(render_list.begin(), render_list.end(), sort_render_list);
		
	// Main render loop
	glm::vec2 camera_offset{camera.x, camera.y};
	glm::vec2 camera_translation = magnitude_gl_from_screen(camera_offset);
	for (auto& element : render_list) {
		// Swap shader based on flags
		if (has_flag(element.flags, Render_Flags::Highlighted)) {
			if (shader != highlighted_shader) {
				shader->end();
				shader = highlighted_shader;
				shader->begin();
				shader->set_int("sampler", 0);
			}
		}
		else {
			if (shader != textured_shader) {
				shader->end();
				shader = textured_shader;
				shader->begin();
				shader->set_int("sampler", 0);
			}
		}

		glActiveTexture(GL_TEXTURE0);
		element.sprite->atlas->bind();

		// Point the texture coordinates to this sprite's texcoords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), element.sprite->tex_coord_offset);
		glEnableVertexAttribArray(1);
			
		auto& physics_engine = get_physics_engine();
		auto position = physics_engine.get_position(element.entity);
		SRT transform = SRT::no_transform();
		transform.scale = {
			element.sprite->width / internal_resolution_width,
			element.sprite->height / internal_resolution_height
		};
		transform.translate = gl_from_screen({
            position->x,
            position->y
		});
		if (!has_flag(element.flags, Render_Flags::ScreenPosition)) {
			transform.translate += camera_translation;
		}
		auto transform_mat = mat3_from_transform(transform);
		shader->set_mat3("transform", transform_mat);

		shader->check();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	shader->end();
	render_list.clear();

	// Now that the scene is rendered, apply any post processing effects
	if (post_processing_info.type != PostProcessingType::None) {
		glViewport(0, 0, static_cast<GLsizei>(screen_x), static_cast<GLsizei>(screen_y));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, color_buffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), square_tex_coords_offset);
		glEnableVertexAttribArray(1);

		auto& info = post_processing_info;
		if (info.type == PostProcessingType::FadeScreen) {
			shader = shaders.get("fade");
			shader->begin();
			shader->set_int("screen", 0);

			float distance = glm::abs(info.fade_time_remaining - info.total_fade_time);
			float darkness = distance / info.total_fade_time;
			shader->set_float("time", darkness);
			info.fade_time_remaining -= dt;
			if (float_almost_equals(info.fade_time_remaining, 0)) {
				info.type = PostProcessingType::None;
			}
		} else if (info.type == PostProcessingType::BattleTransition) {
			shader = shaders.get("battle_transition");
			shader->begin();
			shader->set_int("screen", GL_TEXTURE0);

			// Bind the texture for the effect
			auto& asset_manager = get_asset_manager();
			
			Sprite* effect = nullptr;
			switch(info.transition_type) {
			case BattleTransition::Horizontal:
				effect = asset_manager.get_asset<Sprite>("horizontal_wipe");
				break;
			};
			glActiveTexture(GL_TEXTURE1);
			effect->atlas->bind();
			shader->set_int("effect", 1);

			shader->set_float("cutoff", info.battle_transition_cutoff / info.battle_transition_time);

			// We're done when the time asked for has elapsed.
			float extra = 1;
			float time_left = info.battle_transition_cutoff - info.battle_transition_time;
			if (float_almost_equals(time_left, extra)) {
				info.type = PostProcessingType::None;
			}

			info.battle_transition_cutoff += dt;
		}
		
		shader->check();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		shader->end();
	}
}

void RenderEngine::render_text(float dt) {
	auto& shaders = get_shader_manager();
	auto shader = shaders.get("text");
	shader->begin();

	// Text is raw 2D, so just use an orthographic projection
	SRT transform = SRT::no_transform();
	glm::mat3 mat = mat3_from_transform(transform);
	shader->set_mat3("transform", mat);
	shader->set_int("sampler", 0);

	glBindBuffer(GL_ARRAY_BUFFER, font_vert_buffer);
	glBindVertexArray(font_vao);

	for (const auto& info : text_infos) {
		auto color = has_flag(info.flags, Text_Flags::Highlighted) ? Colors::TextHighlighted : Colors::TextWhite;
		shader->set_vec3("text_color", color);

		auto px_point = px_from_screen(info.point);
		for (auto c : info.text) {
			Character freetype_char = characters[c];
		
			GLfloat left = static_cast<float>(px_point.x) + freetype_char.px_bearing.x;
			GLfloat right = left + freetype_char.px_size.x;
			GLfloat bottom = static_cast<float>(px_point.y) - (freetype_char.px_size.y - freetype_char.px_bearing.y); // Put the bearing point, not the bottom of the glyph, at requested Y
			GLfloat top = bottom + freetype_char.px_size.y;
			
			gl_unit gl_left = gl_from_screen(screen_x_from_px((pixel_unit)left));
			gl_unit gl_right = gl_from_screen(screen_x_from_px((pixel_unit)right));
			gl_unit gl_bottom = gl_from_screen(screen_y_from_px((pixel_unit)bottom));
			gl_unit gl_top = gl_from_screen(screen_y_from_px((pixel_unit)top));
			
			// FreeType loads the fonts upside down, which is why texture coordinates look wonky
			GLfloat vertices[12][2] = {
				// Vertices 
				{ gl_left,  gl_top },
				{ gl_left,  gl_bottom },
				{ gl_right, gl_bottom },
			
				{ gl_left,  gl_top },
				{ gl_right, gl_bottom },
				{ gl_right, gl_top },
			
				// Texture coordinates
				{ 0.f, 0.f },
				{ 0.f, 1.f },
				{ 1.f, 1.f },
			
				{ 0.f, 0.f },
				{ 1.f, 1.f },
				{ 1.f, 0.f }
			};
			
			// Render glyph texture over quad
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, freetype_char.texture);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			
			shader->check();
			glDrawArrays(GL_TRIANGLES, 0, 6);
		
			px_point.x += (freetype_char.advance / 64);
		}
	}
	shader->end();
	text_infos.clear();
}

void RenderEngine::render_primitives(float dt) {
	for (auto& draw_func : primitives) {
		draw_func();
	}
	primitives.clear();
}
