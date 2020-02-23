struct Entity {
	static int next_id;
	int id;
	std::string name;
	std::map<const std::type_info*, ComponentHandle> components;
	
	// Component functions	
	template <typename Component_Type>
	void remove_component();
	
	template <typename Component_Type>
	Component_Type* get_component();
	
	Component* get_component(std::string kind);
	
	void clear_components();
	
	static EntityHandle create(std::string name);
	static void destroy(EntityHandle handle);
	
	TableNode* save();
	void load(TableNode* self);

	void imgui_visualizer();
};
int Entity::next_id = 0;

Pool<Entity, DEFAULT_POOL_SIZE> entity_pool;

// @spader 3/19/19: Not 100% sure I want to have the hero be
// a special thing in the global scope, but no reason not to at this point.
EntityHandle g_hero;
void init_hero();
