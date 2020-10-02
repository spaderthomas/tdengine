EntityID create_entity(std::string name);
void destroy_entity(int entity);
void draw_entity(int entity, Render_Flags flags = Render_Flags::None);
void move_entity(int entity);
void teleport_entity(int entity, float x, float y);
void register_collider(int entity);

void register_animation(std::string name, std::vector<std::string> frames);
std::vector<std::string> get_frames(std::string animation_name);

void enable_input_channel(int channel);
void disable_input_channel(int channel);
bool was_key_pressed(GLFW_KEY_TYPE id, int mask = INPUT_MASK_NONE);
bool is_key_down(GLFW_KEY_TYPE id, int mask = INPUT_MASK_NONE);
bool was_chord_pressed(GLFW_KEY_TYPE mod_key, GLFW_KEY_TYPE cmd_key, int mask = INPUT_MASK_NONE);

float get_camera_x();
float get_camera_y();
void move_camera(float x, float y);

Sprite* get_frame(std::string animation, int frame);

bool draw_sprite_button(std::string sprite, float sx, float sy);

// @spader 2020/08/13: These just forward to the internal draw functions.
// They are here because Lua can't use GLM vectors.
void _draw_line_from_points(float p1x, float p1y, float p2x, float p2y, float r, float g, float b, float a);
void _draw_rect_filled_screen(float origin_x, float origin_y, float extent_x, float extent_y, float r, float g, float b, float a);
void _draw_rect_outline_screen(float origin_x, float origin_y, float extent_x, float extent_y, float r, float g, float b, float a);
void _draw_rect_outline_world(float origin_x, float origin_y, float extent_x, float extent_y, float r, float g, float b, float a);

void toggle_console();

void register_lua_api();
