struct Entity_Tree {
	string dir;
	vector<pool_handle<Entity>> entities;
	vector<Entity_Tree*> children;
	int size = 0;
	
	static Entity_Tree* create(string dir);
	pool_handle<Entity> find(string name);
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
	void ExecCommand(char* command_line);
	
	static int TextEditCallbackStub(ImGuiTextEditCallbackData* data); 
	int TextEditCallback(ImGuiTextEditCallbackData* data);
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

struct Layer {
	Input input;
	Level* active_level; 
	
	virtual void update(float dt) = 0;
	virtual void exec_console_cmd(char* cmd) {}
	virtual void render() {}
	virtual void init() {}
};

struct Battle : Layer {
	EntityHandle battlers[2];
	
	void init() override;
	void render() override;
	void update(float dt) override;
};
Battle battle;

enum Editor_State {
	IDLE,
	INSERT,
	EDIT,
	DRAG,
	RECTANGLE_SELECT
};
enum Selection_Kind {
	NONE,
	TILE,
	ENTITY
};
struct Editor : Layer {
	// Stores tiles in a tree structure for good display purposes.
	Entity_Tree* tile_tree;
	void draw_tile_tree(Entity_Tree* root);
	int draw_tile_tree_recursive(Entity_Tree* root, int unique_btn_index);
	Editor_State state = IDLE;
	Selection_Kind kind = NONE;
	TaskEditor task_editor;
	
	// State for moving things around cleanly
	glm::ivec2 last_grid_drawn = { 0, 0 };
	glm::vec2 top_left_drag = { 0.f, 0.f };
	glm::vec2 smooth_drag_offset = { 0.f, 0.f };
	
	// State about selected thing
	EntityHandle selected;
	void translate();
	void delete_selected();
	void draw_component_editor();
	
	vector<function<void()>> action_stack;
	void undo_action();
	vector<function<void()>> mark_stack;
	void undo_mark();
	
	
	// Big stuff
	void reload_lua();
	void reload_assets();
	void reload_everything();
	void exec_console_cmd(char* cmd) override;
	void update(float dt) override;
	void render() override;
	void init() override;
	Console console;
};
Editor editor;

// @spader 3/19/19: This enum is bad
enum Game_State {
	GAME = 0,
	DIALOGUE = 1
};
struct Game : Layer {
	Dialogue_Tree* active_dialogue;
	Particle_System particle_system;
	Console console;
	
	bool in_dialogue = false;
	
	void update(float dt) override;
	void render() override;
	void init() override;
};
Game game;

int iactive_layer = 0;
vector<Layer*> all_layers = { &editor, &game, &battle};
Layer* active_layer = &editor;


