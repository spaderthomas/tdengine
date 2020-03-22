struct Texture;
struct Sprite : Asset {
	int height = 0;
	int width = 0;
	int num_channels = 0;
	
	Texture* atlas = nullptr;
	std::vector<float> tex_coords;
	GLvoid* tex_coord_offset = nullptr;

	static GLuint vert_buffer;
	static GLuint elem_buffer;
	static GLuint vao;

	bool is_initialized() const;
};

// OpenGL 
GLuint Sprite::vert_buffer;
GLuint Sprite::elem_buffer;
GLuint Sprite::vao;

void bind_sprite_buffers();
