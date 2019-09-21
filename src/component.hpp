struct Component {
	virtual TableNode* save() const;
	virtual void load(TableNode* table) { cout << "Tried to call virtual load() on base Component"; };
	virtual string name() { return "Component"; };
	virtual void imgui_visualizer() = 0;

	// @spader 8/23/2019: Make init_from_table virtual
};
struct Graphic_Component : Component {
	vector<Animation*> animations;
	Animation* active_animation = nullptr;
	glm::vec2 scale;
	int z;

	void begin_animation(string wish);
	void add_animation(Animation* anim);
	Sprite* get_current_frame();
	void init_from_table(TableNode* table);
	string name() override;
	void imgui_visualizer() override;
};
struct Position_Component : Component {
	glm::vec2 world_pos = glm::vec2(0.f);
	
	TableNode* save() const override;
	void load(TableNode* self) override;
	string name() override;
	void imgui_visualizer() override;
};
struct Movement_Component : Component {
	glm::vec2 speed;
	glm::vec2 wish;
	void init_from_table(TableNode* table);
	string name() override;
	void imgui_visualizer() override;
};
struct Vision_Component : Component {
	float width;
	float depth;
	
	void init_from_table(TableNode* table);
	string name() override;
	void imgui_visualizer() override;
};
struct Interaction_Component : Component {
	bool was_interacted_with = false;
	EntityHandle other;
	
	string name() override;
	void imgui_visualizer() override;
};
struct Door_Component : Component {
	string level;
	glm::vec2 position;
	
	void init_from_table(TableNode* table);
	TableNode* save() const override;
	void load(TableNode* self);
	string name() override;
	void imgui_visualizer() override;
};
enum Collider_Kind : int;
struct Collision_Component : Component {
	struct {
		glm::vec2 screen_center;
		glm::vec2 screen_extents;
	} bounding_box;
	Collider_Kind kind;
	
	void init_from_table(TableNode* table);
	string name() override;
	void imgui_visualizer() override;
};
struct Task_Component : Component {
	Task* task;
	
	string name() override;
	void init_from_table(TableNode* table);
	void imgui_visualizer() override;
};
struct BattleComponent : Component {
	unsigned health;
	vector<Move*> moves;
	
	void init_from_table(TableNode* table);
	string name() override;
	void imgui_visualizer() override;
};
struct TileComponent : Component {
	int x;
	int y;
	
	string name() override;
	TableNode* save() const override;
	void load(TableNode* self);
	void imgui_visualizer() override;
};

//@metaprogramming
union any_component {
	Graphic_Component graphic_component;
	Position_Component position_component;
	Movement_Component movement_component;
	Vision_Component vision;
	Interaction_Component interaction_component;
	Door_Component door_component;
	Collision_Component collision_component;
	Task_Component task_component;
	BattleComponent battle_component;
	TileComponent tile_component;
	
	any_component() {} // Necessary so we can in place new components in the pool.
};

Pool<any_component, DEFAULT_POOL_SIZE> component_pool;
typedef pool_handle<any_component> ComponentHandle;

unordered_map<string, const type_info*> component_map = {
	{ "Graphic_Component", &typeid(Graphic_Component) },
	{ "Position_Component", &typeid(Position_Component) },
	{ "Movement_Component", &typeid(Movement_Component) },
	{ "Vision", &typeid(Vision_Component) },
	{ "Interaction_Component", &typeid(Interaction_Component) },
	{ "Door_Component", &typeid(Door_Component) },
	{ "Task_Component", &typeid(Task_Component) },
	{ "BattleComponent", &typeid(BattleComponent) },
	{ "TileComponent", &typeid(TileComponent) },
};

#define def_get_cmp(var, entity, type) type* var = (entity)->get_component<type>()
#define get_cmp(entity, type) (entity)->get_component<type>()
#define def_cast_cmp(varname, cmp, type) type* varname = dynamic_cast<type*>((cmp))
#define if_component(var, entity, type) type* var = (entity)->get_component<type>(); if ((var))
