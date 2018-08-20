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

// Defines a box whose bottom center is at the characters origin. Used for selecting.
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

struct Fake_Component {
	int value;
};

union any_component {
	Graphic_Component graphic_component;
	Position_Component position_component;
	Bounding_Box bounding_box;
	Movement_Component movement_component;
	Vision vision;
	Interaction_Component interaction_component;
	State_Component state_component;
};

struct pool_entry_info {
	bool available : 1;
};
#define POOL_ENTRY_AVAILABLE 0x1
#define POOL_SIZE 1000

typedef int component_handle;

struct {
	any_component* components;
	pool_entry_info* info;

	void init() {
		components = (any_component*)calloc(sizeof(any_component), POOL_SIZE);
		info = (pool_entry_info*)calloc(sizeof(pool_entry_info), POOL_SIZE);
		memset(info, 1, sizeof(pool_entry_info) * POOL_SIZE);
	}

	component_handle next_available() {
		fox_for(handle, POOL_SIZE) {
			pool_entry_info entry = info[handle];
			if (entry.available) {
				mark_unavailable(handle);
				return handle;
			}
		}

		return -1;
	}

	template <typename Component_Type>
	Component_Type* get(component_handle handle) {
		return (Component_Type*)(components + handle);
	}
	Component* get(component_handle handle) {
		return (Component*)(components + handle);
	}

	inline void mark_available(component_handle handle) {
		info[handle].available = true;
		memset(components + handle, 0, sizeof(any_component));
	}
	inline void mark_unavailable(component_handle handle) {
		info[handle].available = false;
	}
} component_pool;

