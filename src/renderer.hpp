struct Sprite;
struct Texture_Atlas;
struct SRT;
struct Render_Info {
	Sprite* sprite;
	SRT transform;
};
struct Renderer {
	unordered_map<Texture_Atlas*, vector<Render_Info>> render_lists;
	void draw_sprite(Sprite* sprite, SRT transform);
	void render_for_frame();

} renderer;
