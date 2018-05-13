struct Sprite;
struct Texture_Atlas;
struct Render_Element {
	Sprite* sprite;
	SRT transform;

	bool operator < (const Render_Element& elem) const {
		return transform.translate.y < elem.transform.translate.y;
	}
};
enum Render_Layer { BACKGROUND, FOREGROUND, NUM_LAYERS };

struct Renderer {
	unordered_map<Texture_Atlas*, vector<Render_Element>> fg_render_lists;
	unordered_map<Texture_Atlas*, vector<Render_Element>> bg_render_lists;
	void draw_sprite(Sprite* sprite, SRT transform, Render_Layer layer);
	void draw_sprite(Sprite* sprite, glm::ivec2 grid_pos, glm::vec2 scale, Render_Layer layer);
	void render_for_frame();

} renderer;
