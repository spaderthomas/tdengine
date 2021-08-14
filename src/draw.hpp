struct Camera {
	float x;
	float y;
	bool snap_to_player = false;
	bool follow_player = false;
};

enum class Render_Flags : int {
	None = 0,
	Highlighted = 1 << 0,
	ScreenPosition = 1 << 1
};
ENABLE_ENUM_FLAG(Render_Flags)

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
ENABLE_ENUM_FLAG(Text_Flags)

struct TextRenderInfo {
	std::string text;
	glm::vec2 point;
	Text_Flags flags;
};

enum class PostProcessingType {
	None = 0,
	FadeScreen = 1,
	BattleTransition = 2,
};

enum class BattleTransition {
	Horizontal = 1
};

struct PostProcessingInfo {
	PostProcessingType type;
	
	float total_fade_time;
	float fade_time_remaining;

	BattleTransition transition_type;
	float battle_transition_time;
	float battle_transition_cutoff;
};

struct RenderEngine {
	std::vector<std::function<void()>> primitives;
	std::vector<Render_Element> render_list;
	std::vector<TextRenderInfo> text_infos;
	Camera camera;

	PostProcessingInfo post_processing_info;
	uint frame_buffer;
	uint color_buffer;
	
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
