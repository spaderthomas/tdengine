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

struct Animation : Asset {
	std::vector<Sprite*> frames;
	int icur_frame = -1;
	float time_to_next_frame = 8.f / 60.f;
	void next_frame();
	void add_frames(TableNode* frames_table);
	void add_frames(std::vector<std::string>& frames_to_add);
	void add_frame(std::string& sprite_name);
	Sprite* get_frame(int frame);
};

