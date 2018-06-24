struct Entity {
	int id;
	static int next_id;
	vector<Component*> components;	

	string lua_id; // The global Lua object which defines this entity

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

	static Entity* create(string lua_id) {
		Entity* entity = new Entity;
		entity->id = next_id++;
		entity->lua_id = lua_id;
		
		sol::table lua_components = Lua.state[lua_id];
		for (auto it : lua_components) {
			string component_type = it.first.as<string>();
			sol::table table = it.second.as<sol::table>();
			if (component_type == "Graphic_Component") {
				Graphic_Component* gc = new Graphic_Component;
				gc->init_from_table(table);
				entity->add_component(gc);
			}
			else if (component_type == "Collision_Component") {
				Collision_Component* cc = new Collision_Component;
				cc->init_from_table(table);
				entity->add_component(cc);
			}
			else if (component_type == "Position_Component") {
				Position_Component* pc = new Position_Component;
				pc->init_from_table(table);
				entity->add_component(pc);
			}
		}

		return entity;
	}

	virtual void draw() const {
		auto graphic_component = get_component<Graphic_Component>();
		auto position_component = get_component<Position_Component>();
		if (graphic_component && position_component) {
			renderer.draw(graphic_component, position_component);
		} else {
			string msg = "Tried to draw entity, but component was undefined. Entity number: " + to_string(id);
			tdns_log.write(msg);
		}
	};

	void save(json& j) {
		for (auto& component : components) {
			component->save(j);
		}
	}

	virtual void save(json& j) const {};
	virtual void load(json& j) {};
};

int Entity::next_id = 0;
