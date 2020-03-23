void draw_square(Center_Box box, glm::vec4 color);
void draw_line_from_points(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);
void draw_line_from_origin(glm::vec2 basis, glm::vec4 color);
void draw_rect_screen(glm::vec2 top_left, glm::vec2 top_right, glm::vec2 bottom_right, glm::vec2 bottom_left, glm::vec4 color);
void draw_rect_screen(Points_Box& points, glm::vec4 color);
void draw_rect_world(Points_Box points, glm::vec4 color) ;
	
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

enum Render_Flags {
	None = 0,
	Highlighted = 1 << 0,
};
struct Render_Element {
	int layer;
	float world_pos[2];
	float scale[2];
	Sprite* sprite;
	Render_Flags flags;
};

struct RenderEngine {
	std::vector<std::function<void()>> primitives;
	std::vector<Render_Element> render_list;
	glm::vec2 camera_offset;
	
	void render();
};

RenderEngine& get_render_engine();
