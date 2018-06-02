struct Graphic_Component;
struct Position_Component;

struct Render_Element {
	Graphic_Component* gc;
	Position_Component* pc;
};
enum Render_Layer { BACKGROUND, FOREGROUND, NUM_LAYERS };

struct Renderer {
	vector<Render_Element> render_list;
	void draw(Graphic_Component* gc, Position_Component* pc);
	void render_for_frame();
} renderer;
