struct Camera {
	float x;
	float y;
	EntityID following;
	bool panning = false;
};

enum class Render_Flags : int {
	None = 0,
	Highlighted = 1 << 0,
	ScreenPosition = 1 << 1
};
template <> struct is_flag<Render_Flags> : std::true_type { };
struct Render_Element {
	int layer;
	int entity;
	Sprite* sprite;
	Render_Flags flags;
};

enum class Text_Flags : int {
	None        = 0,
	Highlighted = 1 << 0
};
template <> struct is_flag<Text_Flags> : std::true_type { };
struct TextRenderInfo {
	std::string text;
	glm::vec2 point;
	Text_Flags flags;
};

struct RenderEngine {
	std::vector<std::function<void()>> primitives;
	std::vector<Render_Element> render_list;
	std::vector<TextRenderInfo> text_infos;
	Camera camera;

	bool is_fading = false;
	float fade_time_remaining = 0.f;
	float fade_time;
	uint frame_buffer;
	uint color_buffer;
	uint depth_buffer;

	void init();
	void remove_entity(int entity);
	void render(float dt);
	void render_text(float dt);
	void render_scene(float dt);
	void render_primitives(float dt);
	Camera& get_camera();
};

RenderEngine& get_render_engine();

void init_gl();
void draw_line_from_points(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);
void draw_line_from_origin(glm::vec2 basis, glm::vec4 color);
void draw_rect_filled_screen(glm::vec2 origin, glm::vec2 extents, glm::vec4 color);
void draw_rect_outline_screen(glm::vec2 origin, glm::vec2 extents, glm::vec4 color);
void draw_rect_outline_screen(glm::vec2 tl, glm::vec2 tr, glm::vec2 br, glm::vec2 bl, glm::vec4 color);
void draw_rect_outline_world(glm::vec2 origin, glm::vec2 extents, glm::vec4 color);
void draw_text(std::string text, glm::vec2 point, Text_Flags flags);
