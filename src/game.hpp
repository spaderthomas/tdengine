struct Entity_Tree {
	string dir;
	vector<Entity*> entities;
	vector<Entity_Tree*> children;
	int size = 0;

	static Entity_Tree* create(string dir);
	Entity* find(string lua_id);
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
	Entity* boon;
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


struct Game {
	Entity_Tree* tile_tree;
	glm::ivec2 last_grid_pos_drawn;
	glm::vec2 top_left_drag;
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

		glm::vec2 offset_from_mouse = glm::vec2(0.f);
		Entity* entity;

		// Translates the selection to be under the mouse (grid or free)
		// offset_from_mouse used so it doesnt jerk to be centered under the mouse, but rather just follow it
		void translate_entity() {
			glm::vec2 draggable_position;
			glm::ivec2 grid_pos = grid_pos_from_px_pos(game_input.px_pos) + camera;
			if (snap_to_grid) { 
				draggable_position = screen_from_grid(grid_pos);
			}
			else {
				draggable_position = game_input.screen_pos + offset_from_mouse;
			}

			Position_Component* pc = entity->get_component<Position_Component>();
			pc->screen_pos = draggable_position;
		}

		void draw_component_editor() {
			// Leave the main tdengine window, and pop up a properties window for this entity
			ImGui::End();
			ImGui::Begin("components", 0, ImGuiWindowFlags_AlwaysAutoResize);
			for (auto& component : entity->components) {
				if (dynamic_cast<Graphic_Component*>(component)) {
					if (ImGui::TreeNode("Graphic Component")) {
						Graphic_Component* gc = dynamic_cast<Graphic_Component*>(component);
						Animation* current_animation = gc->active_animation;
						if (ImGui::BeginCombo("Animations", current_animation->name.c_str(), 0)) {
							for (auto anim : gc->animations) {
								bool is_selected = anim->name == current_animation->name;
								if (ImGui::Selectable(anim->name.c_str(), is_selected)) {
									gc->set_animation(anim->name);
								}
								if (is_selected) {
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
						ImGui::SliderFloat2("Scale", glm::value_ptr(gc->scale), 0.f, 1.f);
						ImGui::TreePop();
					}
				}
			}
			ImGui::End();
			ImGui::Begin("tdengine", 0, ImGuiWindowFlags_AlwaysAutoResize);
		}
	} editor_selection;

	vector<function<void()>> stack;

	Level dude_ranch;
	Level cantina;
	Level* active_level;
	Console console;

	Particle_System particle_system;
	Text_Box text_box;

	void reload();
	void undo();
	void exec_console_command(const char* command_line);
	void init();
	void update(float dt);
	void render();
};
Game game_layer;
