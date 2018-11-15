struct Component {
	virtual void save(json& j) const;
	virtual void load(json& j) { cout << "base"; };
	virtual void init_from_table(sol::table table);
	virtual string name() { return "Component"; };
};
struct Graphic_Component : Component {
	vector<Animation*> animations;
	Animation* active_animation = nullptr;
	glm::vec2 scale;
	int z;

	void add_animation(Animation* anim);
	Sprite* get_current_frame();
	void init_from_table(sol::table gc) override;
	string name() override;
};
struct Position_Component : Component {
	glm::vec2 world_pos = glm::vec2(0.f);
	glm::vec2 scale = glm::vec2(1.f);

	void save(json& j) const override;
	void load(json& j) override;
	string name() override;
};
struct Movement_Component : Component {
	glm::vec2 wish;
	string name() override;
};
struct Vision_Component : Component {
	// Defines the dimensions for a box whose bottom/top center (depending on orientation) 
	// is the position (i.e. center) of the entity
	float width;
	float depth;

	void init_from_table(sol::table table);
	string name() override;
};
struct Interaction_Component : Component {
	sol::function on_interact;

	void init_from_table(sol::table table) override;
	string name() override;
};
struct State_Component : Component {
	vector<string> watched_variables;
	vector<string> states;
	string current_state;
	sol::function update;

	void init_from_table(sol::table table) override;
	void set_state(string state);
	string name() override;
};
struct Door_Component : Component {
	string to;
	glm::vec2 position;
	
	void save(json& j) const override;
	void load(json& j);
	string name() override;
};
enum Collider_Kind {
	STATIC,
	DYNAMIC,
	NO_COLLIDER
};
struct Collision_Component : Component {
	struct {
		glm::vec2 screen_center;
		glm::vec2 screen_extents;
	} bounding_box;
	Collider_Kind kind;
	sol::function on_collide;
	
	void init_from_table(sol::table table) override;
	string name() override;
};
struct Action_Component : Component {
	sol::function update;

	void init_from_table(sol::table table) override;
	string name() override;
};
struct Update_Component : Component {
	sol::function update;

	void init_from_table(sol::table table) override;
	string name() override;
};

union any_component {
	Graphic_Component graphic_component;
	Position_Component position_component;
	Movement_Component movement_component;
	Vision_Component vision;
	Interaction_Component interaction_component;
	State_Component state_component;
	Door_Component door_component;
	Collision_Component collision_component;
	Action_Component action_component;
	Update_Component update_component;

	any_component() {} // Necessary so we can in place new components in the pool.
	static void fake_destructor_for_sol(any_component* me) {};
};

Pool<any_component, DEFAULT_POOL_SIZE> component_pool;
typedef pool_handle<any_component> ComponentHandle;

//@metaprogramming
unordered_map<string, const type_info*> component_map = {
	{ "Graphic_Component", &typeid(Graphic_Component) },
	{ "Position_Component", &typeid(Position_Component) },
	{ "Movement_Component", &typeid(Movement_Component) },
	{ "Vision", &typeid(Vision_Component) },
	{ "Interaction_Component", &typeid(Interaction_Component) },
	{ "State_Component", &typeid(State_Component) },
	{ "Door_Component", &typeid(Door_Component) },
	{ "Action_Component", &typeid(Action_Component) },
	{ "Update_Component", &typeid(Update_Component) },
};

#define def_get_cmp(var, entity, type) type* var = (entity)->get_component<type>()
#define get_cmp(entity, type) (entity)()->get_component<type>()
#define def_cast_cmp(varname, cmp, type) type* varname = dynamic_cast<type*>((cmp))