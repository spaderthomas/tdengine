struct Entity {
	int id;
	static int next_id;
	vector<Component*> components;

	void add_component(Component* c) {
		components.push_back(c);
	}

	virtual void draw() const {};
	virtual void save(json& j) const {};
	virtual void load(json& j) {};
};
int Entity::next_id = 0;

struct Tree : Entity {
	Graphic_Component* gc;
	SRT transform;
	
	static Tree* create() {
		Tree* new_tree = new Tree;
		new_tree->id = Entity::next_id++;
		new_tree->transform = SRT::no_transform();
		new_tree->gc = new Graphic_Component;
		new_tree->gc->load_animations_from_lua(Lua.state["tree"]["Graphic_Component"]);
		return new_tree;
	}

	void draw() const override { renderer.draw(gc->get_current_frame(), transform); }

	void save(json& j) const override {
		string type_name = string(typeid(this).name());
		fixup_type_name(type_name);
		j["type"] = type_name;
		transform.save(j["transform"]);
	}

	void load(json& j) override {
		transform.translate.x = j["transform"]["translate.x"];
		transform.translate.y = j["transform"]["translate.y"];
		transform.translate.x = j["transform"]["translate.z"];

		transform.scale.x = j["transform"]["scale.x"];
		transform.scale.y = j["transform"]["scale.y"];

		transform.rad_rot = j["transform"]["rad_rot"];
	}
};

struct Player {
	Graphic_Component* gc;
	SRT transform;

	static Player* create() {
		Player* new_player = new Player;
		new_player->gc = new Graphic_Component;
		new_player->transform = SRT::no_transform();

		sol::table animations = Lua.state["boon"]["Graphic_Component"]["Animations"];

	}
};

