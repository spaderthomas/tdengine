struct Entity {
	int id;
	static int next_id;
	string lua_id; // The global Lua object which defines this entity
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


	// Lifetime functions
	static sol::table get_definition(string lua_id);
	
	static pool_handle<Entity> create(string lua_id);
	static void destroy(pool_handle<Entity> handle);

	void save(json& j);
	void load(json& entity);


	void draw(Render_Flags flags);
};
int Entity::next_id = 0;

Pool<Entity, DEFAULT_POOL_SIZE> entity_pool;
typedef pool_handle<Entity> EntityHandle;

string entity_name(EntityHandle entity);
int entity_id(EntityHandle entity); 
void on_collide(EntityHandle me, EntityHandle other);
void update_animation(EntityHandle me, float dt);
int collider_kind(EntityHandle me);