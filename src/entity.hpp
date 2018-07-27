struct Entity {
	int id;
	static int next_id;
	vector<Component*> components;	

	string lua_id; // The global Lua object which defines this entity

	template <typename Component_Type>
	void add_component(Component_Type* c);

	template <typename Component_Type>
	bool remove_component();

	template <typename Component_Type>
	Component_Type* get_component() const;

	static sol::table get_definition(string lua_id);
	
	static Entity* create(json& entity_json);
	static Entity* create(string lua_id);

	void draw(Render_Flags flags) const;
	void save(json& j);
	void load(json& entity);
};
int Entity::next_id = 0;

