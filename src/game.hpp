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
	Camera camera;
	Console console;

	virtual void enter() { }
	virtual void exit() {  }
	virtual void update(float dt) = 0;
	virtual void reload() {}
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

struct Cutscene {
	string name;
	Level* level;

	Task task;
	int frame = 0;
	bool done = false;

	void init(string name, TableNode* table);
	void update(float dt);
};



struct Game : Layer {
	Dialogue_Tree* active_dialogue;
	Particle_System particle_system;
	
	bool in_dialogue = false;
	
	void update(float dt) override;
	void render() override;
	void init() override;
};
Game game;

struct Cutscene_Thing : Layer {
	unordered_map<string, Cutscene*> cutscenes;
	Cutscene* active_cutscene;

	void exit() override;
	void do_cutscene(string which);
	void reload() override;
	void update(float dt) override;
	void render() override;
	void init() override;
	void exec_console_cmd(char* cmd) override;
};
Cutscene_Thing cutscene_thing;