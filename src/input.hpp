enum Input_ID {
	TDNS_MOUSE_LEFT,
	TDNS_KEY_UP,
	TDNS_KEY_DOWN,
	TDNS_KEY_LEFT,
	TDNS_KEY_RIGHT,
	TDNS_KEY_1,
	TDNS_KEY_2,
	TDNS_KEY_3,
	TDNS_KEY_TAB,
	TDNS_KEY_W,
	TDNS_KEY_A,
	TDNS_KEY_S,
	TDNS_KEY_D,
	COUNT_INPUT_IDS,
};

struct Input {
	bool should_update;
	glm::vec2 px_pos;
	glm::vec2 screen_pos; // Top left is (0,0), bottom right is (1,1)

	bool is_down[COUNT_INPUT_IDS];
	bool was_down[COUNT_INPUT_IDS];

	bool was_pressed(Input_ID id) {
		return is_down[id] && !was_down[id];
	}

	void reset_for_next_frame() {
		fox_for(input_id, COUNT_INPUT_IDS) {
			was_down[input_id] = is_down[input_id];
		}
	}

	glm::vec2 screen_pos_as_gl_coords() {
		return glm::vec2(screen_pos.x * 2 - 1, 1 - screen_pos.y * 2);
	}

	glm::ivec2 grid_pos() {
		int closest_x = floor(px_pos.x / CELL_SIZE);
		int closest_y = floor(px_pos.y / CELL_SIZE);
		return glm::ivec2(closest_x, closest_y);
	}
};
Input global_input;
Input game_input;
Input gui_input;
bool game_input_active = true;

void copy_input(Input& source_input, Input& dest_input) {
	memcpy(dest_input.is_down, source_input.is_down, COUNT_INPUT_IDS);
	memcpy(dest_input.was_down, source_input.was_down, COUNT_INPUT_IDS);
	dest_input.screen_pos = source_input.screen_pos;
	dest_input.px_pos = source_input.px_pos;
}

// GLFW Callbacks
static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	global_input.px_pos = glm::vec2(xpos, ypos);
	global_input.screen_pos = glm::vec2(xpos / SCREEN_X, ypos / SCREEN_Y);
}

#define activate_key(btn_or_key, glfw_key, tdns_key) else if (btn_or_key == glfw_key) { \
                                                         if (action == GLFW_PRESS) { global_input.is_down[tdns_key] = true; } \
                                                         if (action == GLFW_RELEASE) { global_input.is_down[tdns_key] = false; } \
                                                     }
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (0) {}
	activate_key(button, GLFW_MOUSE_BUTTON_LEFT, TDNS_MOUSE_LEFT)
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (0) {} // Just for the above macro
	activate_key(key, GLFW_KEY_UP, TDNS_KEY_UP)
	activate_key(key, GLFW_KEY_DOWN, TDNS_KEY_DOWN)
	activate_key(key, GLFW_KEY_LEFT, TDNS_KEY_LEFT)
	activate_key(key, GLFW_KEY_RIGHT, TDNS_KEY_RIGHT)
	activate_key(key, GLFW_KEY_1, TDNS_KEY_1)
	activate_key(key, GLFW_KEY_2, TDNS_KEY_2)
	activate_key(key, GLFW_KEY_3, TDNS_KEY_3)
	activate_key(key, GLFW_KEY_TAB, TDNS_KEY_TAB)
	activate_key(key, GLFW_KEY_W, TDNS_KEY_W)
	activate_key(key, GLFW_KEY_A, TDNS_KEY_A)
	activate_key(key, GLFW_KEY_S, TDNS_KEY_S)
	activate_key(key, GLFW_KEY_D, TDNS_KEY_D)
}
