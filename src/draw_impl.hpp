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
		//glm::vec2 camera_translation = magnitude_gl_from_screen(glm::vec2(.5, .5) - _camera.offset);
		transform.scale = p2 - p1;
		transform.translate = glm::vec3(p1, 1.f);
		//transform.translate += camera_translation;
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

void Line_Set::set_max_point() {
	max_point = 0;
	for (auto& line : lines) {
		max_point += line.size();
	}
}

int Line_Set::count() {
	return (int)lines.size();
}

void Line_Set::add(std::string line) {
	lines.push_back(line);
}

std::string& Line_Set::operator[](int i) {
	fox_assert(i < (int)lines.size());
	return lines[i];
}

Line_Set& Text_Box::current_set() {
	return sets[index_current_line_set];
}
void Text_Box::begin(std::string text) {
	this->text = text;
	index_current_line_set = 0;
	waiting = false;
	active = true;
	sets.clear();
	
	gl_unit size_x = 1.8f;
	
	// Start the text at the top left of the text box, padded
	gl_unit padding_x = .075f;
	
	// Wrap once you go the fraction of the screen defined by size (minus padding on both sides)
	pixel_unit wrap = (pixel_unit)(screen_x * magnitude_screen_from_gl(size_x - 2 * padding_x));
	
	// Break that bad boy up into lines
	Line_Set set;
	std::string word;
	std::string cur_line;
	subpixel_unit this_word_x = 0;
	subpixel_unit this_line_x = 0;
	Character space = characters[' '];
	fox_for(ichar, text.size()) {
		char c = text[ichar];
		if (is_newline(c)) {
			cur_line += word;
			set.add(cur_line);
			if (set.count() == 3) {
				set.set_max_point();
				sets.push_back(set);
				set.lines.clear();
			}
			
			// Reset everything for the next line
			cur_line = word = "";
			this_line_x = this_word_x = 0;
		}
		// If we find a space, check to see if this word fits on the line.
		else if (is_space(c)) {
			// If it fits, add it and a trailing space
			if (this_word_x + this_line_x <= wrap) {
				cur_line += word + " ";
				this_line_x += this_word_x;
				this_line_x += (space.advance / 64) * scale;
			}
			// If it doesn't, add this line to the set and start a new line.
			else {
				cur_line.pop_back(); // remove trailing space
				set.add(cur_line);
				if (set.count() == 3) {
					set.set_max_point();
					sets.push_back(set);
					set.lines.clear();
				}
				
				// New line starts with the word that spilled us over
				cur_line = word + " ";
				this_line_x = this_word_x;
				this_line_x += (space.advance / 64) * scale;
			}
			
			word.clear();
			this_word_x = 0;
		}
		else {
			Character freetype_char = characters[c];
			this_word_x += (freetype_char.advance / 64) * scale;
			word.push_back(c);
		}
	}
	
	// Add the last trailing lineset
	cur_line += word;
	set.add(cur_line);
	set.set_max_point();
	sets.push_back(set);
}
void Text_Box::update(float dt) {
	if (!active) { 
		time_since_last_update = 0.0f;
		return;
	}
	
	time_since_last_update += dt;
	if (!waiting && time_since_last_update >= seconds_per_update * 4) {
		Line_Set& set = current_set();
		set.point = fox_min(set.point + 1, set.max_point);
		
		// If we've drawn all the text in the set, wait for input to continue.
		if (set.point == set.max_point) {
			waiting = true;
		}
	}
}
void Text_Box::resume() {
	if (!waiting) { return; }
	
	// Try to advance to the next line set. If it doesn't exist, hide the text box.
	index_current_line_set++;
	waiting = false;
	if (index_current_line_set >= (int)sets.size()) {
		index_current_line_set = -1;
		reset_and_hide();
	}
}
void Text_Box::render() {
	if (text == "") { return; }
	
	// Draw the box
	glBindVertexArray(text_box_vao);
	glBindBuffer(GL_ARRAY_BUFFER, text_box_vert_buffer);
	auto& asset_manager = get_asset_manager();
	auto texture = asset_manager.get_asset<Texture>("text_box.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->handle);
	
	textured_shader.begin();
	
	gl_unit size_x = 1.8f, size_y = .5f;
	gl_unit bottom = -.9f, left = -.9f;
	gl_unit top = bottom + size_y;
	gl_unit right = left + size_x;
	
	GLfloat vertices[12][2] = {
		// Vertices 
		{ left,  top },
		{ left,  bottom },
		{ right, bottom },
		
		{ left,  top },
		{ right, bottom },
		{ right, top },
		
		// Texture coordinates
		{ 0.f, 0.f },
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		
		{ 0.f, 0.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f }
	};
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
	
	SRT transform = SRT::no_transform();
	auto transform_mat = mat3_from_transform(transform);
	textured_shader.set_mat3("transform", transform_mat);
	
	textured_shader.set_int("sampler", 0);
	
	textured_shader.check();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	textured_shader.end();
	
	
	// Draw the text
	// Start the text at the top left of the text box, padded
	gl_unit padding_x = .075f;
	gl_unit padding_y = .05f;
	gl_unit text_start_x = left + padding_x;
	gl_unit text_start_y = top - padding_y;
	
	// Move the text down so the top of the text is aligned with the top of the box
	screen_unit screen_align_offset = screen_y_from_px(Character::px_largest.y);
	gl_unit gl_align_offset = 2 * screen_align_offset;
	text_start_y -= gl_align_offset * scale;
	glm::ivec2 text_start_px = px_coords_from_gl_coords(glm::vec2((float)text_start_x, (float)text_start_y));
	
	
	text_shader.begin();
	float cur_subpixel_y = (float)text_start_px.y;
	int characters_drawn = 0;
	Line_Set set = sets[index_current_line_set];
	for (int iline = 0; iline < (int)set.count(); iline++) {
		auto& line = set[iline];
		// Text is raw 2D, so just use an orthographic projection
		SRT transform = SRT::no_transform();
		glm::mat3 mat = mat3_from_transform(transform);
		text_shader.set_mat3("transform", mat);
		text_shader.set_vec3("text_color", Colors::TextWhite);
		text_shader.set_int("sampler", 0);
		
		// We can render text at floating point pixel levels, so explicitly cast
		float cur_subpixel_x = (float)text_start_px.x;
		glBindBuffer(GL_ARRAY_BUFFER, font_vert_buffer);
		glBindVertexArray(font_vao);
		for (auto it = line.begin(); it != line.end(); it++) {
			Character freetype_char = characters[*it];
			
			GLfloat left = cur_subpixel_x + freetype_char.px_bearing.x * scale;
			GLfloat right = left + freetype_char.px_size.x * scale;
			GLfloat bottom = cur_subpixel_y - (freetype_char.px_size.y - freetype_char.px_bearing.y) * scale; // Put the bearing point, not the bottom of the glyph, at requested Y
			GLfloat top = bottom + freetype_char.px_size.y * scale;
			
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
			
			text_shader.check();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			
			cur_subpixel_x += (freetype_char.advance / 64) * scale;
			
			// Increment the character until it reaches the point.
			characters_drawn++;
			if (characters_drawn > set.point) {
				goto end; // Break out of the nested loop
			}
		}
		
		cur_subpixel_y -= Character::px_largest.y * scale;
	}
	
	// Label so we can break to this from the inner loop
	end: text_shader.end();
}
void Text_Box::reset_and_hide() {
	waiting = false;
	active = false;
	text = "";
	sets.clear();
}
bool Text_Box::is_all_text_displayed() {
	bool on_last_set = (index_current_line_set + 1 == sets.size());
	return on_last_set && waiting;
}
bool Text_Box::is_current_set_displayed() {
	Line_Set& set = current_set();
	return (set.point == set.max_point) && waiting;
}

RenderEngine& get_render_engine() {
	static RenderEngine engine;
	return engine;
}

Camera& RenderEngine::get_camera() {
	return camera;
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
		auto sort_by_world_pos = [](const auto& a, const auto& b) {
			return a.world_pos[1] > b.world_pos[1]; 
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
		
			SRT transform = SRT::no_transform();
			transform.scale = {
				r.scale[0],
				r.scale[1]
			};
			transform.translate = gl_from_screen({
                r.world_pos[0],
				r.world_pos[1]
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
