struct Texture;
struct Sprite : Asset {
	int height, width, num_channels;
	Texture* atlas;
	vector<float> tex_coords;
	GLvoid* tex_coord_offset;

	static GLuint vert_buffer;
	static GLuint elem_buffer;
	static GLuint vao;
};

// OpenGL 
GLuint Sprite::vert_buffer;
GLuint Sprite::elem_buffer;
GLuint Sprite::vao;


