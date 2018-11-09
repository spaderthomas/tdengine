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


// Lua exports
string entity_name(EntityHandle entity);
int entity_id(EntityHandle entity); 
void on_collide(EntityHandle me, EntityHandle other);
void update_animation(EntityHandle me, float dt);
int collider_kind(EntityHandle me);
void draw_entity(EntityHandle me, Render_Flags flags);
void teleport_entity(EntityHandle me, float x, float y);
void move_entity(EntityHandle me, float dx, float dy);
// Sets the new animation unconditionally
void set_animation(EntityHandle me, string animation);
// Checks whether the entity is already in that animation, and if so does not reset to frame 1
void set_animation2(EntityHandle me, string animation);
void update_entity(EntityHandle me, float dt);
bool are_interacting(EntityHandle initiator, EntityHandle receiver);
void debug(EntityHandle me);
