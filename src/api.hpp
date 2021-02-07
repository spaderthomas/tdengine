EntityID create_entity(std::string name);
void destroy_entity(int entity);
void draw_entity(int entity);
void move_entity(int entity);
void teleport_entity(int entity, float x, float y);
void register_collider(int entity);
int ray_cast(float x, float y);
sol::object sprite_size(std::string name);

void register_animation(std::string name, std::vector<std::string> frames);
std::vector<std::string> get_frames(std::string animation_name);
Sprite* get_frame(std::string animation, int frame);

void enable_input_channel(int channel);
void disable_input_channel(int channel);
bool was_pressed(GLFW_KEY_TYPE id, int mask = INPUT_MASK_NONE);
bool is_down(GLFW_KEY_TYPE id, int mask = INPUT_MASK_NONE);
bool was_chord_pressed(GLFW_KEY_TYPE mod_key, GLFW_KEY_TYPE cmd_key, int mask = INPUT_MASK_NONE);
float get_cursor_x();
float get_cursor_y();
sol::object screen_dimensions();

float get_camera_x();
float get_camera_y();
void move_camera(float x, float y);

bool draw_sprite_button(std::string sprite, float sx, float sy);
void line_screen(sol::table p1, sol::table p2, sol::table color);
void rect_filled_screen(sol::table rect, sol::table color);
void rect_outline_screen(sol::table rect, sol::table color);
void rect_outline_world(sol::table rect, sol::table color);
void lua_draw_text(std::string text, float x, float y, int flags);

void toggle_console();
void use_layout(const char* name);
void save_layout(const char* name);

void register_lua_api();

void text_box_begin(std::string text);
bool text_box_is_done();
bool text_box_is_active();
bool text_box_is_waiting();
void text_box_resume();
void text_box_skip();
void text_box_highlight_line(int line);
void text_box_add_choice(std::string choice);
