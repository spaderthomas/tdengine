struct Entity {
	int id;
	static int next_id;
	string lua_id; // The global Lua object which defines this entity
	unordered_map<const type_info*, component_handle> components;

	template <typename Component_Type>
	component_handle add_component();

	template <typename Component_Type>
	bool remove_component();

	template <typename Component_Type>
	Component_Type* get_component();

	static sol::table get_definition(string lua_id);
	
	static Entity* create(json& entity_json);
	static Entity* create(string lua_id);

	void draw(Render_Flags flags);
	void save(json& j);
	void load(json& entity);
};
int Entity::next_id = 0;

