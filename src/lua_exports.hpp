// Level
Level* get_level(string name);
void add_entity(Level* level, EntityHandle entity);

// Entity
string entity_name(EntityHandle entity);
int entity_id(EntityHandle entity);
void on_collide(EntityHandle me, EntityHandle other);
void update_animation(EntityHandle me, float dt);
int collider_kind(EntityHandle me);
void draw_entity(EntityHandle me, Render_Flags flags);
void teleport_entity(EntityHandle me, float x, float y);
void move_entity(EntityHandle me, float dx, float dy);
void set_animation(EntityHandle me, string animation);
void set_animation_no_reset(EntityHandle me, string animation);
void update_entity(EntityHandle me, float dt);
bool are_interacting(EntityHandle initiator, EntityHandle receiver);

// Input
bool lua_is_down(GLFW_KEY_TYPE key);
bool lua_was_down(GLFW_KEY_TYPE key);
bool lua_was_pressed(GLFW_KEY_TYPE key);

// Game
void go_through_door(string dest_level);
void begin_dialogue(EntityHandle entity, string scene);