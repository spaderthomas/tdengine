struct Entity {
	int id;
	static int next_id;
	vector<Component*> components;

	void add_component(Component* c) {
		components.push_back(c);
	}

	template <typename Component_Type>
	Component_Type* get_component() const {
		for (auto it = components.begin(); it != components.end(); ++it) {
			Component_Type* cast_component = dynamic_cast<Component_Type*>(*it);
			if (cast_component) { return cast_component; }
		}

		string msg = "Entity " + to_string(id) + " tried to get a component it did not have.";
		msg +=       " Component type: " + string(typeid(Component_Type).name());
		tdns_log.write(msg);
		return nullptr;
	}

	virtual void draw() const {};
	virtual void save(json& j) const {};
	virtual void load(json& j) {};
};
int Entity::next_id = 0;

struct Tree : Entity {
	static Tree* create() {
		Tree* new_tree = new Tree;
		new_tree->id = Entity::next_id++;
		Position_Component* pos = new Position_Component;
		pos->transform = SRT::no_transform();
		new_tree->add_component(pos);
		Graphic_Component* graphic_component = new Graphic_Component;
		graphic_component->load_animations_from_lua(Lua.state["tree"]["Graphic_Component"]);
		new_tree->add_component(graphic_component);
		return new_tree;
	}

	void draw() const override { 
		auto graphic_component = get_component<Graphic_Component>();
		auto position_component = get_component<Position_Component>();
		renderer.draw(graphic_component->get_current_frame(), position_component->transform);
	}

	void save(json& j) const override {
		string type_name = string(typeid(this).name());
		fixup_type_name(type_name);
		j["type"] = type_name;
		auto position_component = get_component<Position_Component>();
		position_component->transform.save(j["transform"]);
	}

	void load(json& j) override {
		auto position_component = get_component<Position_Component>();

		position_component->transform.translate.x = j["transform"]["translate.x"];
		position_component->transform.translate.y = j["transform"]["translate.y"];
		position_component->transform.translate.x = j["transform"]["translate.z"];

		position_component->transform.scale.x = j["transform"]["scale.x"];
		position_component->transform.scale.y = j["transform"]["scale.y"];

		position_component->transform.rad_rot = j["transform"]["rad_rot"];
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

