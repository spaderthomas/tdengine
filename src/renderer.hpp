struct Sprite;
struct Texture_Atlas;
struct Render_Element {
	Sprite* sprite;
	SRT transform;

	// We want to render in Y-descending order so everything overlaps properly
	bool operator < (const Render_Element& elem) const {
		return transform.translate.y > elem.transform.translate.y;
	}
};
enum Render_Layer { BACKGROUND, FOREGROUND, NUM_LAYERS };

struct Renderer {
	vector<Render_Element> render_list;
	void draw(Sprite* sprite, SRT transform);
	void render_for_frame();
} renderer;
