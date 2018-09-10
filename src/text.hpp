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
	if (FT_New_Face(freetype, "C:/Windows/Fonts/Arial.ttf", 0, &face)) {
		tdns_log.write("Failed to load font (yes, even Papyrus failed :(");
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
			(uint)face->glyph->advance.x
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

struct Line_Set {
	vector<string> lines;
	int point = 0;
	int max_point = 0;

	void set_max_point() {
		max_point = 0;
		for (auto& line : lines) {
			max_point += line.size();
		}
	}

	int count() {
		return (int)lines.size();
	}
	
	void add(string line) {
		lines.push_back(line);
	}
	
	string& operator[](int i) {
		fox_assert(i < lines.size());
		return lines[i];
	}
};

struct Text_Box {
	string text;
	bool waiting = false;
	bool active = false;
	float scale = 1.f;

	vector<Line_Set> sets;
	int index_current_line_set;

	void begin(string text);
	void update(int frame);
	void resume();
	void render();
	void reset_and_hide();
	bool is_all_text_displayed();
	bool is_current_set_displayed();
	Line_Set& current_set();
};
