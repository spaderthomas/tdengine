struct Component {
	std::string name;
	int entity;
	int id;
	
	static int next_id;

	virtual void update(float dt);
	std::string get_name();
	int get_id();
	int get_entity();
};
int Component::next_id = 0;

using EntityID = int;

struct Entity {
	static int next_id;
	int id;
	std::string name;
	std::map<std::string, Component*> components;

	Entity(std::string name, int id);
	void update(float dt);
	
	Component* add_component(std::string name);
	Component* get_component(std::string name);
	bool has_component(std::string name);
	void remove_component(std::string name);
	std::vector<Component*> all_components();
	
	int get_id();
	std::string get_name();
	std::string debug_string();
};
int Entity::next_id = 0;

struct EntityManager {
	std::map<int, std::unique_ptr<Entity>> entities;
	int player = -1;
	
	Entity* get_entity(int id);
	bool has_entity(int id);
	int create_entity(std::string name);
	void destroy_entity(int id);
};
EntityManager& get_entity_manager();

struct ComponentManager {
	std::map<int, std::unique_ptr<Component>> components;
	
	Component* get_component(int id);
	bool has_component(int id);
	Component* create_component(std::string name, int entity);
	void destroy_component(int id);
};
ComponentManager& get_component_manager();

struct CutsceneManager {
	void update(float dt);
};
CutsceneManager& get_cutscene_manager();

struct UpdateSystem {
	void run_collision_callbacks(float dt);
	void run_interaction_callback(float dt);
	void run_entity_updates(float dt);
	void do_paused_update(float dt);
};
UpdateSystem& get_update_system();
