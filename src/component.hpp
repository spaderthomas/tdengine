struct Component {
	virtual void save(json& j) const;
	virtual void init_from_table(sol::table table);
	virtual string name() { return "Component"; };
};
struct Graphic_Component : Component {
	vector<Animation*> animations;
	Animation* active_animation = nullptr;
	glm::vec2 scale;
	int z;

	void set_animation(const string wish_name);
	void set_animation2(const string wish_name);
	void add_animation(Animation* anim);
	Sprite* get_current_frame();
	void init_from_table(sol::table gc) override;
	string name() override;
};
struct Position_Component : Component {
	glm::vec2 screen_pos = glm::vec2(0.f);
	glm::vec2 scale = glm::vec2(0.f);

	void save(json& j) const override;
	void load(json& j);
	string name() override;
};
struct Bounding_Box : Component {
	glm::vec2 screen_center;
	glm::vec2 screen_extents;

	void init_from_table(sol::table table) override;
	string name() override;
};
struct Movement_Component : Component {
	glm::vec2 wish;
	string name() override;
};
struct Vision : Component {
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

union any_component {
	Graphic_Component graphic_component;
	Position_Component position_component;
	Bounding_Box bounding_box;
	Movement_Component movement_component;
	Vision vision;
	Interaction_Component interaction_component;
	State_Component state_component;

	any_component() {} // Necessary so we can in place new components in the pool.
};

Pool<any_component, 1000> component_pool;
