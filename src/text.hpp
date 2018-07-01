// bearing.y is the distance from the top of the glyph to the baseline
// bearing.x is the distance from the left of the bounding box to the start of the glpyh
// FreeType stores the advance as 1/64 subpixel values i.e. a value of 64 implies a 1 pixel advance
struct Character {
	GLuint texture;
	glm::ivec2 px_size;
	glm::ivec2 px_bearing; 
	uint advance;

	static glm::ivec2 largest; // Useful to guarantee characters stay inside an area
};
glm::ivec2 Character::largest;
map<GLchar, Character> characters;

FT_Library freetype;
FT_Face face;
GLuint font_vao, font_vert_buffer;
#define PX_FONT_SIZE 16
void init_freetype() {
	if (FT_Init_FreeType(&freetype)) {
		tdns_log.write("Failed to initialize FreeType");
		exit(0);
	}
	if (FT_New_Face(freetype, "C:/Windows/Fonts/PxPlus_IBM_VGA8.ttf", 0, &face)) {
		tdns_log.write("Failed to load font");
		exit(0);
	}

	FT_Set_Pixel_Sizes(face, 0, PX_FONT_SIZE);
	Character::largest = glm::ivec2(0);
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

		if (character.px_size.x > Character::largest.x) {
			Character::largest.x = character.px_size.x;
		}
		if (character.px_size.y > Character::largest.y) {
			Character::largest.y = character.px_size.y;
		}
	}

	FT_Done_Face(face);
	FT_Done_FreeType(freetype);

	glGenVertexArrays(1, &font_vao);
	glGenBuffers(1, &font_vert_buffer);
	glBindVertexArray(font_vao);
	glBindBuffer(GL_ARRAY_BUFFER, font_vert_buffer);

	// FreeType generates vertices for us. We'll pass them in as array of structs of vertices and texture coordinates
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void render_text(pixel_unit x, pixel_unit y, glm::vec3 color, string text) {
	// We can render text at floating point pixel levels, so explicitly cast
	float subpixel_x = (float)x;
	float subpixel_y = (float)y;

	text_shader.begin();
	text_shader.set_vec3("text_color", color);
	float scale = 2;

	// Text is raw 2D, so just use an orthographic projection
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_X), 0.0f, static_cast<GLfloat>(SCREEN_Y));
	text_shader.set_mat4("projection", projection);
	
	glBindVertexArray(font_vao);
	for (auto it = text.begin(); it != text.end(); it++) {
		Character freetype_char = characters[*it];

		GLfloat left = subpixel_x + freetype_char.px_bearing.x * scale;
		GLfloat right = left + freetype_char.px_size.x * scale;
		GLfloat bottom = subpixel_y - (freetype_char.px_size.y - freetype_char.px_bearing.y) * scale; // Put the bearing point, not the bottom of the glyph, at requested Y
		GLfloat top = bottom + freetype_char.px_size.y * scale;

		// FreeType loads the fonts upside down, which is why texture coordinates look wonky
		GLfloat vertices[6][4] = {
			{ left,  top,     0.0, 0.0 },
			{ left,  bottom,  0.0, 1.0 },	
			{ right, bottom,  1.0, 1.0 },

			{ left,  top,     0.0, 0.0 },
			{ right, bottom,  1.0, 1.0 },
			{ right, top,     1.0, 0.0 }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, freetype_char.texture);
		glBindBuffer(GL_ARRAY_BUFFER, font_vert_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		text_shader.check();
		glDrawArrays(GL_TRIANGLES, 0, 6);

		subpixel_x += (freetype_char.advance / 64) * scale; 
	}
	text_shader.end();
}
