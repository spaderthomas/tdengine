void Text_Box::begin(string text) {
	this->text = text;
	point = 0;
	start_line = 0;
	waiting = false;
	lines.clear();

	gl_unit size_x = 1.8f, size_y = .5f;
	gl_unit bottom = -.9f, left = -.9f;
	gl_unit top = bottom + size_y;
	gl_unit right = left + size_x;

	// Start the text at the top left of the text box, padded
	gl_unit padding_x = .075f;

	// Wrap once you go the fraction of the screen defined by size (minus padding on both sides)
	pixel_unit wrap = (pixel_unit)(SCREEN_X * magnitude_screen_from_gl(size_x - 2 * padding_x));

	// Break that bad boy up into lines
	vector<string> words = split(text, ' ');
	subpixel_unit accumulated_x = 0;
	string cur_line;
	for (auto& word : words) {
		subpixel_unit this_word_x = 0;
		for (auto c : word) {
			Character freetype_char = characters[c];
			this_word_x += (freetype_char.advance / 64) * scale;
		}

		Character space = characters[' '];
		this_word_x += (space.advance / 64) * scale;

		accumulated_x += this_word_x;
		if (accumulated_x >= wrap) {
			// Get rid of trailing space
			cur_line.pop_back();
			//this_word_x -= (freetype_char.advance / 64) * scale;
			// Add line and reset, reset for next line
			lines.push_back(cur_line);
			cur_line = word + " ";
			accumulated_x = this_word_x;
		}
		else {
			cur_line += word + " ";
		}
	}

	lines.push_back(cur_line);
}

void Text_Box::update(int frame) {
	if (!waiting && frame % 2) {
		point = fox_min(point + 1, (int)text.size() - 1);
	}
}

// Advance forward three lines and move the point to the beginning. If you go past the last line, reset.
void Text_Box::unwait() {
	if (!waiting) { return; }

	start_line += 3;
#pragma warning(push)
#pragma warning(disable: 4018) // signed/unsigned mismatch
	if (start_line >= lines.size()) {
#pragma warning(pop)
		point = -1;
		start_line = -1;
		waiting = false;
		text = "";
		lines.clear();
	}
	else {
		point = 0;
		waiting = false;
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
#pragma warning(push)
#pragma warning(disable: 4018) // signed/unsigned mismatch
	for (int iline = start_line; iline < lines.size(); iline++) {
#pragma warning(pop)
		auto& line = lines[iline];
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
			if (characters_drawn > point) {
				goto end;
			}
		}

		cur_subpixel_y -= Character::px_largest.y * scale;

		// If we've fully rendered three lines, stop and wait for the signal to continue
		if (!((iline + 1) % 3)) {  // Add 1 because of zero-indexing
			waiting = true;
			goto end;
		}
		// If you're at the end of the last line, wait
		if (iline == lines.size() - 1) {
			waiting = true;
			goto end;
		}
	}

	// Label so we can break to this from both the inner and outer loop
	end: text_shader.end();
}
