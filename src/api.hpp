void draw_entity(int entity, Render_Flags flags = Render_Flags::None);
void move_entity(int entity, float x, float y);

void register_animation(std::string name, std::vector<std::string> frames);
std::vector<std::string> get_frames(std::string animation_name);

void enable_input_channel(int channel);
void disable_input_channel(int channel);
bool was_key_pressed(GLFW_KEY_TYPE id, int mask = INPUT_MASK_NONE);
bool is_key_down(GLFW_KEY_TYPE id, int mask = INPUT_MASK_NONE);
bool was_chord_pressed(GLFW_KEY_TYPE mod_key, GLFW_KEY_TYPE cmd_key, int mask = INPUT_MASK_NONE);

void set_camera_offset(float x, float y);

Sprite* get_frame(std::string animation, int frame);
