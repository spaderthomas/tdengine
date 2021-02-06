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
std::map<GLchar, Character> characters;

FT_Library freetype;
FT_Face face;
GLuint font_vao, font_vert_buffer, text_box_vao, text_box_vert_buffer;
#define PX_FONT_SIZE 20

uint advance(char c);
void init_fonts();
