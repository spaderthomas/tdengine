struct Asset {
	string name;
};

struct Texture;
struct Mesh;
struct Shader;
struct Animation;
struct Asset_Table {
	vector<Texture*> textures;
	vector<Mesh*> meshes;
	vector<Shader*> shaders;
	vector<Animation*> animations;

	Texture* get_texture(string id);
	Animation* get_animation(string id);
	Mesh* get_mesh(string id);
};

Asset_Table asset_table;
