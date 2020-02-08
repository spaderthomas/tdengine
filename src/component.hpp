struct Component {
	virtual TableNode* save() const { return nullptr; }
	virtual TableNode* make_template() const { return new TableNode; }
	
	virtual void init(TableNode* table) {};
	virtual void load(TableNode* table) { cout << "Tried to call virtual load() on base Component"; };
	virtual string name() { return "Component"; };
	virtual void imgui_visualizer() = 0;
};


struct LuaComponent {
	string name;
	int entity;
	
	static LuaComponent create(string name, int entity);
	virtual void update(float dt);	
};

struct LuaEntity {
	static int next_id;
	int id;
	string name;
	map<string, LuaComponent> components;

	LuaEntity(string name, int id);
	void update(float dt);
	void add_component(string name);
	int get_id();
};
int LuaEntity::next_id = 0;

struct EntityManager;
struct LuaEntityHandle {
	int id;
	EntityManager* manager;

	LuaEntity* operator->() const;
	LuaEntity* get() const;
};

struct EntityManager {
	LuaEntity* get_entity(int id);
	bool has_entity(int id);
	LuaEntityHandle create_entity(string name);

	map<int, unique_ptr<LuaEntity>> entities;
};


