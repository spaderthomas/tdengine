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

// Each of these basic tiles needs a custom save to mark down the subclass, then it just 
// calls into Basic_Tile::save() for the standard stuff 
#define DEFINE_TILE(Type, lua_id) struct Type : Basic_Tile { \
	static Type* create() { \
		Type* new_ent = new Type; \
		new_ent->id = Entity::next_id++; \
		new_ent->init(lua_id); \
		return new_ent; \
	} \
    \
	void save(json& j) const override { \
		string type_name = string(typeid(this).name()); \
		fixup_type_name(type_name); \
		j["type"] = type_name; \
		\
		Basic_Tile::save(j); \
	} \
};

DEFINE_TILE(Tree, "tree")
DEFINE_TILE(Grass, "grass")
DEFINE_TILE(GrassFlower1, "grass_flower1")
DEFINE_TILE(GrassFlower2, "grass_flower2")
DEFINE_TILE(GrassFlower3, "grass_flower3")
DEFINE_TILE(Fence, "fence")
DEFINE_TILE(Sand, "sand")
DEFINE_TILE(Sand_Cracked, "sand_cracked")
#if 0
struct Tree : Basic_Tile {
 	static Tree* create() {
		Tree* new_tree = new Tree;
		new_tree->id = Entity::next_id++;
		new_tree->init("tree");
		return new_tree;
	}

	void save(json& j) const override {
		string type_name = string(typeid(this).name());
		fixup_type_name(type_name);
		j["type"] = type_name;

		Basic_Tile::save(j);
	}
};
struct Grass : Basic_Tile {
	static Grass* create() {
		Grass* grass = new Grass;
		grass->id = Entity::next_id++;
		grass->init("grass");
		return grass;
	}

	void save(json& j) const override {
		string type_name = string(typeid(this).name());
		fixup_type_name(type_name);
		j["type"] = type_name;

		Basic_Tile::save(j);
	}
};
struct GrassFlower1 : Basic_Tile {
	static GrassFlower1* create() {
		GrassFlower1* grass = new GrassFlower1;
		grass->id = Entity::next_id++;
		grass->init("grass_flower1");
		return grass;
	}

	void save(json& j) const override {
		string type_name = string(typeid(this).name());
		fixup_type_name(type_name);
		j["type"] = type_name;

		Basic_Tile::save(j);
	}
};
struct GrassFlower2 : Basic_Tile {
	static GrassFlower2* create() {
		GrassFlower2* grass = new GrassFlower2;
		grass->id = Entity::next_id++;
		grass->init("grass_flower2");
		return grass;
	}

	void save(json& j) const override {
		string type_name = string(typeid(this).name());
		fixup_type_name(type_name);
		j["type"] = type_name;

		Basic_Tile::save(j);
	}
};
struct GrassFlower3 : Basic_Tile {
	static GrassFlower3* create() {
		GrassFlower3 *grass = new GrassFlower3;
		grass->id = Entity::next_id++;
		grass->init("grass_flower3");
		return grass;
	}

	void save(json& j) const override {
		string type_name = string(typeid(this).name());
		fixup_type_name(type_name);
		j["type"] = type_name;

		Basic_Tile::save(j);
	}
};
struct Fence : Basic_Tile {
	static Fence* create() {
		Fence* new_ent = new Fence;
		new_ent->id = Entity::next_id++;
		new_ent->init("fence");
		return new_ent;
	}

	void save(json& j) const override {
		string type_name = string(typeid(this).name());
		fixup_type_name(type_name);
		j["type"] = type_name;

		Basic_Tile::save(j);
	}
};
struct Sand : Basic_Tile {
	static Sand* create() {
		Sand* new_ent = new Sand;
		new_ent->id = Entity::next_id++;
		new_ent->init("sand");
		return new_ent;
	}

	void save(json& j) const override {
		string type_name = string(typeid(this).name());
		fixup_type_name(type_name);
		j["type"] = type_name;

		Basic_Tile::save(j);
	}
};
struct Sand_Cracked : Basic_Tile {
	static Sand_Cracked* create() {
		Sand_Cracked* new_ent = new Sand_Cracked;
		new_ent->id = Entity::next_id++;
		new_ent->init("sand_cracked");
		return new_ent;
	}

	void save(json& j) const override {
		string type_name = string(typeid(this).name());
		fixup_type_name(type_name);
		j["type"] = type_name;

		Basic_Tile::save(j);
	}
};
#endif
