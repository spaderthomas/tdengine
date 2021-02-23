namespace API {
int         alloc_entity(std::string name);
void        free_entity(int entity);
std::string entity_name(int entity);
bool        has_component(int entity, const char* name);
int         get_component(int entity, const char* name);
int         add_component(int entity, const char* name);
sol::object all_components(int entity);
int         alloc_component(int entity, const char* name);
void        free_component(int component);
std::string component_name(int component);
void        draw_entity(int component);
void        move_entity(int entity);
void        teleport_entity(int entity, float x, float y);
void        register_position(int entity);
void        register_collider(int entity);
void        register_raycastable(int entity);
void        disable_collision_detection(int entity);
void        register_player(int entity);
void        do_interaction_check();
void        register_interactable(int entity);
void        register_watcher();
sol::object ray_cast(float x, float y);
sol::object sprite_size(std::string name);
void        register_animation(std::string name, std::vector<std::string> frames);
void        enable_input_channel(int channel);
void        disable_input_channel(int channel);
bool        was_pressed(GLFW_KEY_TYPE id, int mask = INPUT_MASK_NONE);
bool        is_down(GLFW_KEY_TYPE id, int mask = INPUT_MASK_NONE);
bool        was_chord_pressed(GLFW_KEY_TYPE mod_key, GLFW_KEY_TYPE cmd_key, int mask = INPUT_MASK_NONE);
sol::object screen_dimensions();
sol::object cursor();
sol::object camera();
void        move_camera(float x, float y);
bool        draw_sprite_button(std::string sprite, float sx, float sy);
void        line_screen(sol::table p1, sol::table p2, sol::table color);
void        rect_filled_screen(sol::table rect, sol::table color);
void        rect_outline_screen(sol::table rect, sol::table color);
void        rect_outline_world(sol::table rect, sol::table color);
void        draw_text(std::string text, float x, float y, int flags);
void        toggle_console();
void        use_layout(const char* name);
void        save_layout(const char* name);
int         count_frames(std::string animation);
void        screen(const char* dimension);
void        log(const char* message, uint8_t flags);
}
void register_lua_api();
