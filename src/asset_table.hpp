struct Asset {
	string name;
};

struct Sprite;
struct Mesh;
struct Shader;
struct Texture_Atlas;
struct Animation;
struct Asset_Table {
	vector<Sprite*> sprites;
	vector<Mesh*> meshes;
	vector<Shader*> shaders;
	vector<Texture_Atlas*> atlases;
	vector<Animation*> animations;

	Sprite* get_sprite(string id);
	Texture_Atlas* get_texture_atlas(string id);
	Animation* get_animation(string id);
	Mesh* get_mesh(string id);
};

Asset_Table asset_table;
