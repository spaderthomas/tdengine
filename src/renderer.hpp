struct Graphic_Component;
struct Position_Component;

enum Render_Flags {
	None = 0,
	Highlighted = 1 << 0,
};
struct Render_Element {
	Graphic_Component* gc;
	Position_Component* pc;
	EntityHandle e;
	Render_Flags flags;
};

struct Renderer {
	std::vector<std::function<void()>> primitives;
	std::vector<Render_Element> render_list;
	void draw(Graphic_Component* gc, Position_Component* pc, EntityHandle e, Render_Flags flags);
	void render_for_frame();
} renderer;

