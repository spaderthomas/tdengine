void init_gl();

void draw_line_from_points(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);
void draw_line_from_origin(glm::vec2 basis, glm::vec4 color);
void draw_rect_filled_screen(glm::vec2 origin, glm::vec2 extents, glm::vec4 color);
void draw_rect_outline_screen(glm::vec2 origin, glm::vec2 extents, glm::vec4 color);
void draw_rect_outline_screen(glm::vec2 tl, glm::vec2 tr, glm::vec2 br, glm::vec2 bl, glm::vec4 color);
void draw_rect_outline_world(glm::vec2 origin, glm::vec2 extents, glm::vec4 color);

struct Camera {
	float x;
	float y;
	EntityID following;
	bool panning = false;
};

enum Render_Flags {
	None = 0,
	Highlighted = 1 << 0,
	ScreenPosition = 1 << 1
};
struct Render_Element {
	int layer;
	int entity;
	Sprite* sprite;
	Render_Flags flags;
};

struct RenderEngine {
	std::vector<std::function<void()>> primitives;
	std::vector<Render_Element> render_list;
	Camera camera;

	void remove_entity(int entity);
	void render();
	Camera& get_camera();
};

RenderEngine& get_render_engine();
