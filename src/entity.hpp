struct Component {
	std::string name;
	int entity;
	int id;
	
	static int next_id;

	static Component* create(std::string name, int entity);
	virtual void update(float dt);
	std::string get_name();
	int get_id();
	int get_entity();
};
int Component::next_id = 0;

struct Entity {
	static int next_id;
	int id;
	std::string name;
	std::map<std::string, Component*> components;

	Entity(std::string name, int id);
	void update(float dt);
	Component* add_component(std::string name);
	Component* get_component(std::string name);
	int get_id();
	std::string get_name();
};
int Entity::next_id = 0;

struct EntityManager;
struct EntityHandle {
	int id;

	operator bool() const;
	Entity* operator->() const;
	Entity* get() const;
};

struct EntityManager {
	std::map<int, std::unique_ptr<Entity>> entities;
	
	Entity* get_entity(int id);
	bool has_entity(int id);
	EntityHandle create_entity(std::string name);
	void update(float dt);
};
EntityManager& get_entity_manager();
