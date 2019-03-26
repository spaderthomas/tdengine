// GLFW Callbacks
static void GLFW_Cursor_Pos_Callback(GLFWwindow* window, double xpos, double ypos) {
	xpos = max<double>(xpos, 0); ypos = max<double>(ypos, 0);
	global_input.px_pos = glm::vec2(xpos, SCREEN_Y - ypos);
	global_input.screen_pos = glm::vec2(xpos / SCREEN_X, (SCREEN_Y - ypos) / SCREEN_Y);
	global_input.world_pos = global_input.screen_pos + active_layer->camera.offset;
}

void GLFW_Mouse_Button_Callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			global_input.is_down[GLFW_MOUSE_BUTTON_LEFT] = true;
		}
		if (action == GLFW_RELEASE) {
			global_input.is_down[GLFW_MOUSE_BUTTON_LEFT] = false;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			global_input.is_down[GLFW_MOUSE_BUTTON_RIGHT] = true;
		}
		if (action == GLFW_RELEASE) {
			global_input.is_down[GLFW_MOUSE_BUTTON_RIGHT] = false;
		}
	}
}

void GLFW_Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) { global_input.is_down[key] = true; }
    if (action == GLFW_RELEASE) { global_input.is_down[key] = false; } 
}

void GLFW_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset) {
	global_input.scroll = glm::vec2((float)xoffset, (float)yoffset);
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (float)xoffset;
	io.MouseWheel += (float)yoffset;
}

void GLFW_Error_Callback(int err, const char* msg) {
	cout << err;
	cout << msg;
}
