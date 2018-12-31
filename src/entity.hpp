struct Entity {
	int id;
	static int next_id;
	string lua_id; 
	unordered_map<const type_info*, pool_handle<any_component>> components;

	// Component functions
	template <typename Component_Type>
	pool_handle<any_component> add_component();
	template <typename Component_Type>
	bool remove_component();
	template <typename Component_Type>
	Component_Type* get_component();
	any_component* get_component(string kind);
	void clear_components();

	static EntityHandle create(string lua_id);
	static void destroy(EntityHandle handle);

	void save(json& j);
	void load(json& entity);
};
int Entity::next_id = 0;

Pool<Entity, DEFAULT_POOL_SIZE> entity_pool;
