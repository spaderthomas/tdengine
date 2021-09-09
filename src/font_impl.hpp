void init_font(std::string font_path, int font_size) {
	tdns_log.write("@load_font: " + font_path, Log_Flags::File);
	
	FT_Face face = nullptr;

	//if (FT_New_Face(freetype, font_path.c_str(), 0, &face)) {
	if (FT_New_Face(freetype, font_path.c_str(), 0, &face)) {
		std::string message = "Failed to load font: ";
		message += font_path;
		tdns_log.write(message);
		exit(0);
	}

	auto as_path = std::filesystem::path(font_path);
	
	Font font;
	font.name = as_path.filename().stem();
	
	FT_Set_Pixel_Sizes(face, 0, font_size);
	font.px_largest = glm::ivec2(0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Textures are grayscale i.e. 1 byte per pixel, so turn off 4 byte alignment
	for (GLubyte c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			tdns_log.write("FreeType failed to load character");
			tdns_log.write(std::string("Character was: %c", c));
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
			(uint)face->glyph->advance.x
		};
		font.characters.insert(std::pair<GLchar, Character>(c, character));
		
		if (character.px_size.x > font.px_largest.x) {
			font.px_largest.x = character.px_size.x;
		}
		if (character.px_size.y > font.px_largest.y) {
			font.px_largest.y = character.px_size.y;
		}
	}
	
	FT_Done_Face(face);

	g_fonts[font.name] = font;
}

void init_fonts() {
	if (FT_Init_FreeType(&freetype)) {
		tdns_log.write("Failed to initialize FreeType");
		exit(0);
	}

	init_font(g_dialogue_font_path, g_dialogue_font_size);
	init_font(g_editor_font_path, g_editor_font_size);

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
}

uint Font::advance(char c) {
	if (characters.find(c) == characters.end()) return 0;
	
	auto& character = characters[c];
	return character.advance;
}
