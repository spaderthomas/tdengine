struct Entity {
	int id;
	static int next_id;
	string name;
	unordered_map<const type_info*, pool_handle<any_component>> components;
	
	// Component functions
	template <typename Component_Type>
	pool_handle<any_component> add_component();
	
	template <typename Component_Type>
	void remove_component();
	
	template <typename Component_Type>
	Component_Type* get_component();
	any_component* get_component(string kind);
	void clear_components();
	
	static EntityHandle create(string name);
	static void destroy(EntityHandle handle);
	
	TableNode* save();
	void load(TableNode* self);
};
int Entity::next_id = 0;

Pool<Entity, DEFAULT_POOL_SIZE> entity_pool;

// @spader 3/19/19: Not 100% sure I want to have the hero be
// a special thing in the global scope, but no reason not to at this point.
EntityHandle g_hero;
void init_hero();
