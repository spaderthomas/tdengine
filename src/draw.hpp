void init_gl();

void draw_line_from_points(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);
void draw_line_from_origin(glm::vec2 basis, glm::vec4 color);
void draw_rect_filled_screen(glm::vec2 origin, glm::vec2 extents, glm::vec4 color);
void draw_rect_outline_screen(glm::vec2 origin, glm::vec2 extents, glm::vec4 color);
void draw_rect_outline_screen(glm::vec2 tl, glm::vec2 tr, glm::vec2 br, glm::vec2 bl, glm::vec4 color);
void draw_rect_outline_world(glm::vec2 origin, glm::vec2 extents, glm::vec4 color);
	
struct Line_Set {
	std::vector<std::string> lines;
	int point = 0;
	int max_point = 0;
	
	void set_max_point();
	int count();
	void add(std::string line);
	std::string& operator[](int i);
};

struct Text_Box {
	std::string text;
	bool waiting = false;
	bool active = false;
	float scale = 1.f;
	float time_since_last_update = 0.0f;
	
	std::vector<Line_Set> sets;
	uint index_current_line_set;
	
	void begin(std::string text);
	void update(float dt);
	void resume();
	void render();
	void reset_and_hide();
	bool is_all_text_displayed();
	bool is_current_set_displayed();
	Line_Set& current_set();
};


struct Camera {
	float x;
	float y;
	EntityID following;
	bool panning = false;
};

enum Render_Flags {
	None = 0,
	Highlighted = 1 << 0,
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
	
	void render();
	Camera& get_camera();
};

RenderEngine& get_render_engine();
