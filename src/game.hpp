struct Entity_Tree {
	string dir;
	vector<pool_handle<Entity>> entities;
	vector<Entity_Tree*> children;
	int size = 0;

	static Entity_Tree* create(string dir);
	pool_handle<Entity> find(string lua_id);
}; 

struct Console {
    char                  InputBuf[256];
    ImVector<char*>       Items;
    bool                  ScrollToBottom;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char*> Commands;
	Console();
	~Console();
    
	// Portable helpers
	static int Stricmp(const char* str1, const char* str2);
	static int Strnicmp(const char* str1, const char* str2, int n);
	static char* Strdup(const char *str);
	static void  Strtrim(char* str);

	void ClearLog();
	void AddLog(const char* fmt, ...) IM_FMTARGS(2);
	void Draw(const char* title);
	void ExecCommand(const char* command_line);

	static int TextEditCallbackStub(ImGuiTextEditCallbackData* data); 
	int TextEditCallback(ImGuiTextEditCallbackData* data);
};

enum Facing {
	left,
	right,
	up,
	down
};
struct Player {
	pool_handle<Entity> boon;
	glm::vec2 position = glm::vec2(.25, .25);
	Facing facing;
	float frame_timer = 8.f / 60.f;
	void init();
	void update(float dt);
};
Player player;

struct Circle_Buffer {
	int* data = nullptr;
	int head = 0;
	int capacity = 0;
	int len = 0;

	void push_back(int elem);
	optional<int> pop_front();
	void clear();
};

// amp * sin(period * k - phase_shift)
struct Sine_Func {
	float amp = 1.f;
	float period = 1.f;
	float phase_shift = 1.f;

	float eval_at(float point);
};

struct Particle {
	glm::vec2 velocity = glm::vec2(0.f);
	glm::vec2 position = glm::vec2(0.f);
	glm::vec2 scale = glm::vec2((2 / SCREEN_X), (2 / SCREEN_Y));
	glm::vec4 color = red;
	float life;
	bool alive = false;
	Sine_Func y_offset;
	float y_baseline;
	
	void make_alive();
};

struct Particle_System {
	Circle_Buffer free_list;
	array<Particle, 1024> particles;
	
	void init();
	void start();
	void update(float dt);
};

struct Dialogue_Tree;
struct Dialogue_Node;
struct Game {
	Entity_Tree* tile_tree;
	glm::ivec2 last_grid_pos_drawn;
	glm::vec2 top_left_drag;
	enum Game_State {
		GAME = 0,
		DIALOGUE = 1
	} game_state = GAME;
	enum Editing_State {
		IDLE,
		INSERT,
		EDIT,
		DRAG,
		RECTANGLE_SELECT,
	} editor_state;

	struct Editor_Selection {
		enum Selected_Kind {
			TILE,
			ENTITY
		} kind;

		glm::vec2 smooth_drag_offset = glm::vec2(0.f);
		pool_handle<Entity> selection;
		pool_handle<Entity> operator->() {
			return selection;
		}
		void translate_entity();
		void draw_component_editor();
	} editor_selection;

	vector<function<void()>> stack;

	//@move to a config file
	Level dude_ranch;
	Level cantina;
	Level* active_level;
	Console console;
	Dialogue_Tree* active_dialogue;
	string scene;
	
	Particle_System particle_system;
	Text_Box text_box;

	bool in_dialogue = false;

	void go_through_door(string to);
	void play_intro();
	void begin_dialogue(Entity* entity);
	void reload();
	void undo();
	void exec_console_command(const char* command_line);
	void init();
	void update(float dt);
	void render();
};
Game game_layer;
