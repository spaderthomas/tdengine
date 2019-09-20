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
	void exec_console_cmd(char* cmd) override;
};
Game game;

struct Cutscene_Thing : Layer {
	unordered_map<string, Cutscene*> cutscenes;
	Cutscene* active_cutscene;

	void exit() override;
	bool does_cutscene_exist(const string& which);
	void do_cutscene(string which);
	void reload() override;
	void update(float dt) override;
	void render() override;
	void init() override;
	void exec_console_cmd(char* cmd) override;
};
Cutscene_Thing cutscene_thing;
