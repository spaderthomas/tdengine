EntityID create_entity(std::string name);
void destroy_entity(int entity);
void draw_entity(int entity);
void move_entity(int entity);
void teleport_entity(int entity, float x, float y);
void register_collider(int entity);
int ray_cast(float x, float y);

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

float get_camera_x();
float get_camera_y();
void move_camera(float x, float y);

bool draw_sprite_button(std::string sprite, float sx, float sy);
void line_screen(sol::table p1, sol::table p2, sol::table color);
void rect_filled_screen(sol::table rect, sol::table color);
void rect_outline_screen(sol::table rect, sol::table color);
void rect_outline_world(sol::table rect, sol::table color);

/*
// show grid
float imgui_cursor_pos_x();
float imgui_cursor_pos_y();
float imgui_window_size_x();
float imgui_window_size_y(); 
void imgui_add_line(float xb, float yb, float xe, float ye);
void imgui_channels_split(int count);
void imgui_set_current_channel(int which);
void imgui_add_bezier_curve(sol::table p0, sol::table cp0, sol::table p1, sol::table cp1, sol::table color, float thickness);
ImGui::IsAnyItemActive();
ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
ImGui::BeginGroup();
ImGui::Text("%s", node->Name);
ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
ImGui::ColorEdit3("##color", &node->Color.x);
ImGui::EndGroup();
ImGui::GetItemRectSize();
ImGui::SetCursorScreenPos(node_rect_min);
float imgui_mouse_delta_x();
float imgui_mouse_delta_y();
void imgui_add_circle_filled(); lazy
*/

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
