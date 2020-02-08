// GLFW Callbacks
static void GLFW_Cursor_Pos_Callback(GLFWwindow* window, double xpos, double ypos) {
	xpos = std::max<double>(xpos, 0); ypos = std::max<double>(ypos, 0);
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
	std::cout << err;
	std::cout << msg;
}

void GLFW_Window_Size_Callback(GLFWwindow* window, int width, int height) {
	use_arbitrary_screen_size(height, width);
}

int init_glfw() {
		auto result = glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	
	g_window = glfwCreateWindow((int)SCREEN_X, (int)SCREEN_Y, "tdengine", NULL, NULL);
	if (g_window == NULL) {
		tdns_log.write("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(g_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		tdns_log.write("Failed to initialize GLAD");
		return -1;
	}
	
	glfwSetErrorCallback(GLFW_Error_Callback);
	glfwSetCursorPosCallback(g_window, GLFW_Cursor_Pos_Callback);
	glfwSetMouseButtonCallback(g_window, GLFW_Mouse_Button_Callback);
	glfwSetKeyCallback(g_window, GLFW_Key_Callback);
	glfwSetScrollCallback(g_window, GLFW_Scroll_Callback);
	glfwSetWindowSizeCallback(g_window, GLFW_Window_Size_Callback);
	
	glfwSwapInterval(0);

	return 0;
}
