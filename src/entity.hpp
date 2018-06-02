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
	virtual void save(json& j) const {};
	virtual void load(json& j) {};
};
int Entity::next_id = 0;

// @note
// I was thinking we'd have one such struct for every class of tile
// It could be as broad as you want (e.g. a struct to represent every kind of background tile)
// or as narrow as you want (e.g. a struct to represent one single character).
// In general, though, inheritance isn't going past Entity -> Standard Thing -> Standard Thing with slight modification
struct Basic_Tile : Entity {
	static Basic_Tile* create(string lua_id) {
		Basic_Tile* new_tile = new Basic_Tile;
		new_tile->id = next_id++;
		new_tile->lua_id = lua_id;
		new_tile->init(lua_id);
		return new_tile;
	}

	void init(string lua_id) {
		Graphic_Component* graphic_component = new Graphic_Component;
		graphic_component->load_animations_from_lua(Lua.state[lua_id]["Graphic_Component"]);
		graphic_component->z = Lua.state[lua_id]["Graphic_Component"]["z"];
		this->add_component(graphic_component);

		// We have to do this after GC so we can grab a sprite and use its size
		// @hack Works on the assumptions that all sprites are same dimensions
		Position_Component* pos = new Position_Component;
		pos->transform = SRT::no_transform();
		Sprite* sprite = graphic_component->get_current_frame();
		int tilesize_y = sprite->height / 16;
		int tilesize_x = sprite->width / 16;
		pos->transform.scale = glm::vec2(tilesize_x * SCR_TILESIZE_X, tilesize_y * SCR_TILESIZE_Y);
		this->add_component(pos);
	}

	void save(json& j) const override {

		auto position_component = get_component<Position_Component>();
		if (position_component) {
			position_component->transform.save(j["transform"]);
		}
		else {
			string msg = "Tried to a save a position component, but component was null. Entity ID: " + to_string(id);
			tdns_log.write(msg);
		}
	}

	void load(json& j) override {
		auto position_component = get_component<Position_Component>();

		if (position_component) {
			position_component->transform.translate.x = j["transform"]["translate.x"];
			position_component->transform.translate.y = j["transform"]["translate.y"];
			position_component->transform.translate.z = j["transform"]["translate.z"];

			position_component->transform.scale.x = j["transform"]["scale.x"];
			position_component->transform.scale.y = j["transform"]["scale.y"];

			position_component->transform.rad_rot = j["transform"]["rad_rot"];
		}
		else {
			string msg = "Tried to a load a position component, but component was null. Entity ID: " + to_string(id);
			tdns_log.write(msg);
		}
	}
};
