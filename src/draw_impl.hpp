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
	std::vector<float> tex_coords;
	
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
		solid_shader.begin();
		glm::vec4 color_ = color; //@hack So I can pass as a reference everywhere else. maybe make it a pointer? but then inconsistent :(
		solid_shader.set_vec4("color", color_);

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
		auto transform = SRT::no_transform();
		transform.translate = gl_from_screen(origin);
		transform.scale = extents;
		auto trans_mat = mat3_from_transform(transform);
		solid_shader.begin();
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

RenderEngine& get_render_engine() {
	static RenderEngine engine;
	return engine;
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

void RenderEngine::render() {
	bind_sprite_buffers();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); // Verts always the same (a square)
	glEnableVertexAttribArray(0);

	Shader* shader = &textured_shader;
	shader->begin();
	shader->set_int("sampler", 0);

	// Algorithm:
	// Sort by Z-position (as if you were going to do the Painter's algorithm
	// Z positions are integral (since 2D), so just collect elements of each Z-type into collections
	// Sort these collections by Y-axis (so overlap works properly)
	auto sort_by_z = [](const auto& a, const auto& b) {
		return a.layer < b.layer;
	};
	sort(render_list.begin(), render_list.end(), sort_by_z);


	// @hack do an in place sort
	int layer_max = std::numeric_limits<int>::min();
	std::vector<std::vector<Render_Element>> depth_sorted_render_elements;
	for (auto& render_element : render_list) {
		if (render_element.layer > layer_max) {
			std::vector<Render_Element> new_depth_level;
			depth_sorted_render_elements.push_back(new_depth_level);
			layer_max = render_element.layer;
		}
		depth_sorted_render_elements.back().push_back(render_element);
	}

	// Main render loop
	glm::vec2 camera_offset{camera.x, camera.y};
	glm::vec2 camera_translation = magnitude_gl_from_screen(camera_offset);
	for (auto& depth_level_render_elements : depth_sorted_render_elements) {
		auto sort_by_world_pos = [](const auto& ra, const auto& rb) {
			auto& physics_engine = get_physics_engine();
			auto ca = physics_engine.get_collider(ra.entity);
			auto cb = physics_engine.get_collider(rb.entity);
			return ca->origin.y > cb->origin.y; 
		};
		stable_sort(depth_level_render_elements.begin(), depth_level_render_elements.end(), sort_by_world_pos);
	
		// Draw the correctly sorted elements for a depth level
		for (auto& r : depth_level_render_elements) {
			// Swap shader based on flags
			if (r.flags & Render_Flags::Highlighted) {
				if (shader != &highlighted_shader) {
					shader->end();
					shader = &highlighted_shader;
					shader->begin();
					shader->set_int("sampler", 0);
				}
			}
			else {
				if (shader != &textured_shader) {
					shader->end();
					shader = &textured_shader;
					shader->begin();
					shader->set_int("sampler", 0);
				}
			}

			r.sprite->atlas->bind();

			// Point the texture coordinates to this sprite's texcoords
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), r.sprite->tex_coord_offset);
			glEnableVertexAttribArray(1);
			
			auto& physics_engine = get_physics_engine();
			auto collider = physics_engine.get_collider(r.entity);
			SRT transform = SRT::no_transform();
			transform.scale = {
				r.sprite->width / internal_resolution_width,
				r.sprite->height / internal_resolution_height
			};
			transform.translate = gl_from_screen({
                collider->origin.x,
                collider->origin.y
			});
			transform.translate += camera_translation;
			auto transform_mat = mat3_from_transform(transform);
			shader->set_mat3("transform", transform_mat);

			shader->check();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	shader->end();
	render_list.clear();


	// Finally, render all the primitives on top of the screen in the order they were queued.
	for (auto& draw_func : primitives) {
		draw_func();
	}
	primitives.clear();
}
