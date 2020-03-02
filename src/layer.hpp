struct Level;
struct Layer {
	InputManager input;
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

