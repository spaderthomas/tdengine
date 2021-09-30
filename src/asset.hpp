struct Asset {
	std::string name;

	virtual void make_this_type_polymorphic() {}
};

struct Mesh : Asset {
	std::vector<float> verts;
	std::vector<uint> indices;
	std::vector<float> tex_coords;

	GLvoid* vert_offset;
	GLvoid* indices_offset;
	GLvoid* tex_coord_offset;
	bool use_tex_coords = false;

	static GLuint vert_buffer;
	static GLuint elem_buffer;
	static GLuint vao;

	void bind();
	void draw(GLuint mode);
};

GLuint Mesh::vert_buffer;
GLuint Mesh::elem_buffer;
GLuint Mesh::vao;

void init_mesh();

struct AssetManager {
	std::map<std::string, Asset*> assets;

	template <typename Asset_Type>
	void add_asset(std::string name, Asset_Type* asset);
	
	template <typename Asset_Type>
	Asset_Type* get_asset(std::string name) const;
	
	template <typename Asset_Type>
	std::vector<Asset_Type*> get_all();
};

AssetManager& get_asset_manager();

#define REGULAR_ATLAS_SIZE 2048

// We need this struct so we can identify which rectangle goes to which sprite when we pack them
struct Name_And_ID {
	std::string name;
	int id;
};

struct Texture : Asset {
	GLuint handle;

	int width;
	int height;
	int num_channels;

	void bind();
};

struct Sprite : Asset {
	int height = 0;
	int width = 0;
	int num_channels = 0;
	
	Texture* texture = nullptr;
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

// Animations should live in Lua, and the component should figure out what frame
// should be submitted to draw.
struct Animation : Asset {
	std::vector<std::string> frames;
	void add_frames(std::vector<std::string>& frames_to_add);
	void add_frame(std::string& sprite_name);
	std::string get_frame(int frame);
};

void init_sprites();
void bind_sprite_buffers();
void destroy_sprite(std::string name);
void reload_texture_atlas();
void destroy_texture(std::filesystem::path path);
void create_texture_atlas();
void load_other_textures(const char* directory);
void add_single_sprite(const char* texture_file);
Texture* create_texture(const char* texture_name);
