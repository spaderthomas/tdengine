// bearing.y is the distance from the top of the glyph to the baseline
// bearing.x is the distance from the left of the bounding box to the start of the glpyh
// FreeType stores the advance as 1/64 subpixel values i.e. a value of 64 implies a 1 pixel advance
struct Character {
	GLuint texture;
	glm::ivec2 px_size;
	glm::ivec2 px_bearing;
	uint advance;

	static glm::ivec2 px_largest; // Useful to guarantee characters stay inside an area. 
};
glm::ivec2 Character::px_largest;
map<GLchar, Character> characters;

FT_Library freetype;
FT_Face face;
GLuint font_vao, font_vert_buffer, text_box_vao, text_box_vert_buffer;
#define PX_FONT_SIZE 20
void init_fonts() {
	// FreeType initialization
	if (FT_Init_FreeType(&freetype)) {
		tdns_log.write("Failed to initialize FreeType");
		exit(0);
	}
	if (FT_New_Face(freetype, "C:/Windows/Fonts/Inconsolata-Regular.ttf", 0, &face)) {
		tdns_log.write("Failed to load font");
		exit(0);
	}

	FT_Set_Pixel_Sizes(face, 0, PX_FONT_SIZE);
	Character::px_largest = glm::ivec2(0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Textures are grayscale i.e. 1 byte per pixel, so turn off 4 byte alignment
	for (GLubyte c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			tdns_log.write("FreeType failed to load character");
			tdns_log.write(string("Character was: %c", c));
			exit(0);
		}

		// Generate texture. Note: We're going to load it in as red, then convert to grayscale in the shader.
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		// Some sane defaults
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Save this data and put it in a map
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters.insert(std::pair<GLchar, Character>(c, character));

		if (character.px_size.x > Character::px_largest.x) {
			Character::px_largest.x = character.px_size.x;
		}
		if (character.px_size.y > Character::px_largest.y) {
			Character::px_largest.y = character.px_size.y;
		}
	}

	FT_Done_Face(face);
	FT_Done_FreeType(freetype);

	// GL buffers for text
	glGenVertexArrays(1, &font_vao);
	glGenBuffers(1, &font_vert_buffer);
	glBindVertexArray(font_vao);
	glBindBuffer(GL_ARRAY_BUFFER, font_vert_buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(GLfloat) * 12));

	// GL buffers for the text box
	glGenVertexArrays(1, &text_box_vao);
	glGenBuffers(1, &text_box_vert_buffer);
	glBindVertexArray(text_box_vao);
	glBindBuffer(GL_ARRAY_BUFFER, text_box_vert_buffer);

	// VAO
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(GLfloat) * 12));
}

struct {
	string text;
	int point = -1;
	float scale = 1.f * (SCREEN_X / 640); //@future: If you need bigger text, just load a bigger font. This looks terrible (but fine for now!)

	void begin(string text) {
		this->text = text;
		point = 0;
	}

	void update(int frame) {
		if (frame % 2) {
			point = fox_min(point + 1, (int)text.size() - 1);
			float scale = 1.f * (SCREEN_X / 640);
		}
	}

	void render() {
		if (point == -1 || point == (text.size() - 1)) { return; }
		// Draw the box
		glBindVertexArray(text_box_vao);
		glBindBuffer(GL_ARRAY_BUFFER, text_box_vert_buffer);
		auto texture = asset_table.get_asset<Texture>("test.png");
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

		// Wrap once you go the fraction of the screen defined by size (minus padding on both sides)
		pixel_unit wrap = SCREEN_X * magnitude_screen_from_gl(size_x - 2 * padding_x);

		// Break that bad boy up into lines
		string this_frame_text = text.substr(0, point+1);
		vector<string> words = split(this_frame_text, ' ');
		subpixel_unit accumulated_x = 0;
		string cur_line;
		vector<string> lines;
		for (auto it = words.begin(); it != words.end(); it++) {
			subpixel_unit this_word_x = 0;
			string word = *it;
			for (auto c : word) {
				Character freetype_char = characters[c];
				this_word_x += (freetype_char.advance / 64) * scale;
			}

			Character freetype_char = characters[' '];
			this_word_x += (freetype_char.advance / 64) * scale;

			accumulated_x += this_word_x;
			if (accumulated_x >= wrap) {
				// Get rid of trailing space
				cur_line.pop_back();
				this_word_x -= (freetype_char.advance / 64) * scale;
				// Add line and reset, reset for next line
				lines.push_back(cur_line);
				cur_line = word + " ";
				accumulated_x = this_word_x;
			}
			else {
				cur_line += word + " ";
			}
		}

		// Now we have the final line of text. We know it all fits into one line -- but there is an (evil) special case
		// If we're halfway through a word, we want to start on a new line so we don't abruptly change lines.
		// Get rid of trailing space
		if (cur_line.size()) {
			cur_line.pop_back();
			accumulated_x -= (characters[' '].advance / 64) * scale;
		}

		// None of the bounds checks make sense if the text is empty.
		if (text.size()) {
			// Now we want to collect all of the characters until (and including) the next space, and see if they overflow 
			if (text.at(point) != ' ') {
				int _point = point + 1;
				while (_point < (text.size() - 1) && text.at(_point) != ' ') {
					Character freetype_char = characters[text.at(_point)];
					accumulated_x += (freetype_char.advance / 64) * scale;
					_point++;
				}
				accumulated_x += (characters[' '].advance / 64) * scale;
			}


			// If they do overflow, peel off characters from this line and put them on a new line
			if (accumulated_x >= wrap) {
				string new_final_line;
				int last_line_point = cur_line.size() - 1;
				while (cur_line.at(last_line_point) != ' ' && last_line_point >= 0) {
					char c = cur_line.back(); cur_line.pop_back();
					new_final_line.insert(new_final_line.begin(), c);
					last_line_point--;
				}
				lines.push_back(cur_line);
				lines.push_back(new_final_line);
			} else { // No overflow; not a special case
				lines.push_back(cur_line);
			}
		}

		text_shader.begin();
		float cur_subpixel_y = (float)text_start_px.y;
		for (auto line : lines) {
			// We can render text at floating point pixel levels, so explicitly cast
			float cur_subpixel_x = (float)text_start_px.x;

			text_shader.set_vec3("text_color", white3);
			text_shader.set_int("sampler", 0);

			// Text is raw 2D, so just use an orthographic projection
			SRT transform = SRT::no_transform();
			glm::mat3 mat = mat3_from_transform(transform);
			text_shader.set_mat3("transform", mat);

			glBindBuffer(GL_ARRAY_BUFFER, font_vert_buffer);
			glBindVertexArray(font_vao);
			for (auto it = line.begin(); it != line.end(); it++) {
				Character freetype_char = characters[*it];

				GLfloat left = cur_subpixel_x + freetype_char.px_bearing.x * scale;
				GLfloat right = left + freetype_char.px_size.x * scale;
				GLfloat bottom = cur_subpixel_y - (freetype_char.px_size.y - freetype_char.px_bearing.y) * scale; // Put the bearing point, not the bottom of the glyph, at requested Y
				GLfloat top = bottom + freetype_char.px_size.y * scale;

				gl_unit gl_left = gl_from_screen(screen_x_from_px(left));
				gl_unit gl_right = gl_from_screen(screen_x_from_px(right));
				gl_unit gl_bottom = gl_from_screen(screen_y_from_px(bottom));
				gl_unit gl_top = gl_from_screen(screen_y_from_px(top));

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
				glBindTexture(GL_TEXTURE_2D, freetype_char.texture);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

				text_shader.check();
				glDrawArrays(GL_TRIANGLES, 0, 6);

				cur_subpixel_x += (freetype_char.advance / 64) * scale;
			}
			cur_subpixel_y -= Character::px_largest.y * scale;
		}

		text_shader.end();
	}
} text_box;
