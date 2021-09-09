// bearing.y is the distance from the top of the glyph to the baseline
// bearing.x is the distance from the left of the bounding box to the start of the glpyh
// FreeType stores the advance as 1/64 subpixel values i.e. a value of 64 implies a 1 pixel advance
struct Character {
	GLuint texture;
	glm::ivec2 px_size;
	glm::ivec2 px_bearing;
	uint advance;
	
};


struct Font {
	uint advance(char c);
	
	std::map<GLchar, Character> characters;	
	glm::ivec2 px_largest;
	std::string name;
};

FT_Library freetype;
GLuint font_vao, font_vert_buffer;
std::map<std::string, Font> g_fonts;

void init_fonts();
