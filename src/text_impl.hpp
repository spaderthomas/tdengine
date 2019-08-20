Line_Set& Text_Box::current_set() {
	return sets[index_current_line_set];
}
void Text_Box::begin(string text) {
	this->text = text;
	index_current_line_set = 0;
	waiting = false;
	active = true;
	sets.clear();
	
	gl_unit size_x = 1.8f;
	
	// Start the text at the top left of the text box, padded
	gl_unit padding_x = .075f;
	
	// Wrap once you go the fraction of the screen defined by size (minus padding on both sides)
	pixel_unit wrap = (pixel_unit)(SCREEN_X * magnitude_screen_from_gl(size_x - 2 * padding_x));
	
	// Break that bad boy up into lines
	Line_Set set;
	string word;
	string cur_line;
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
	auto texture = asset_table.get_asset<Texture>("text_box.png");
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
	screen_unit screen_align_offset = Character::px_largest.y / SCREEN_Y;
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
		text_shader.set_vec3("text_color", white3);
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

