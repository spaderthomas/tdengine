struct Entity {
	int id;
	static int next_id;
	vector<Component*> components;	

	string lua_id; // The global Lua object which defines this entity

	template <typename Component_Type>
	void add_component(Component_Type* c) {
		this->remove_component<Component_Type>();
		components.push_back(c);
	}

	template <typename Component_Type>
	bool remove_component() {
		for (auto it = components.begin(); it != components.end(); it++) {
			if (dynamic_cast<Component_Type*>(*it)) {
				components.erase(it);
				return true;
			}
		}

		return false;
	}

	template <typename Component_Type>
	Component_Type* get_component() const {
		for (auto it = components.begin(); it != components.end(); ++it) {
			Component_Type* cast_component = dynamic_cast<Component_Type*>(*it);
			if (cast_component) { return cast_component; }
		}

#if 0
		string msg = "Entity " + to_string(id) + " tried to get a component it did not have.";
		msg +=       " Component type: " + string(typeid(Component_Type).name());
		tdns_log.write(msg);
#endif
		return nullptr;
	}
	
	// Pulls defaults from Lua, then fills in whatever was saved
	static Entity* create(json& entity_json) {
		//@note 
		// if you don't explicitly make this a string, it recursively calls
		// this create (since the result is a json object), then promptly crashes
		string lua_id = entity_json["lua_id"];
		Entity* new_entity = Entity::create(lua_id);
		new_entity->load(entity_json);
		return new_entity;
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
			else if (component_type == "Bounding_Box") {
				Bounding_Box* box = new Bounding_Box;
				box->init_from_table(table);
				entity->add_component(box);
			}
			else if (component_type == "Position_Component") {
				Position_Component* pc = new Position_Component;
				pc->init_from_table(table);
				entity->add_component(pc);
			}
			else if (component_type == "Movement_Component") {
				Movement_Component* mc = new Movement_Component;
				mc->wish = glm::vec2(0.f);
				entity->add_component(mc);
			}
		}

		return entity;
	}

	void draw() const {
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
		j["lua_id"] = lua_id;
		int icomponent = 0;
		for (auto& component : components) {
			component->save(j["Components"][icomponent++]);
		}
	}

	void load(json& entity) {
		auto components_json = entity["Components"];
		for (auto component : components_json) {
			if (component["kind"] == "Position_Component") {
				Position_Component* pc = new Position_Component;
				pc->load(component);
				this->add_component(pc);
			}
		}
	}
};

int Entity::next_id = 0;
