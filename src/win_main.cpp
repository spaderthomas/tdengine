#define _CRT_SECURE_NO_WARNINGS

// Library includes
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb/stb_rect_pack.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H  
}
#include "lua/lua.hpp"
#include "sol/sol.hpp"

#include "imgui/imgui.h"

// STL
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <experimental/filesystem>
#include <typeindex>
using namespace std;


#include "imgui_impl_glfw_gl3.hpp"

#include "log.hpp"
#include "utils.hpp"
#include "tdns_lua.hpp"
#include "data.hpp"
#include "shader.hpp"
#include "transform.hpp"
#include "renderer.hpp"
#include "asset_table.hpp"
#include "sprite.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "animation.hpp"
#include "component.hpp"
#include "entity.hpp"
#include "draw.hpp"
#include "collision.hpp"
#include "serialization.hpp"
#include "tilemap.hpp"
#include "input.hpp"
#include "renderer_functions.hpp"
#include "text.hpp"
#include "game.hpp"



int main() {
	tdns_log.init();
	
	// GLFW INIT
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	GLFWwindow* window = glfwCreateWindow((int)SCREEN_X, (int)SCREEN_Y, "demo", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, GLFW_Cursor_Pos_Callback);
	glfwSetMouseButtonCallback(window, GLFW_Mouse_Button_Callback);
	glfwSetKeyCallback(window, GLFW_Key_Callback);
	glfwSetScrollCallback(window, GLFW_Scroll_Callback);
	glfwSetErrorCallback(GLFW_Error_Callback);

	init_shaders();
	init_mesh();
	for (auto dirname : atlas_folders) {
		create_texture_atlas(dirname);
	}
	Lua.init();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, false);
	ImGui::StyleColorsDark();

	//---OPENGL INIT
	// Set up some debug output
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallbackKHR(gl_debug_callback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &Sprite::vao);
	glGenBuffers(1, &Sprite::vert_buffer);
	glGenBuffers(1, &Sprite::elem_buffer);
	glGenVertexArrays(1, &Mesh::vao);
	glGenBuffers(1, &Mesh::vert_buffer);
	glGenBuffers(1, &Mesh::elem_buffer);

	glBindVertexArray(Sprite::vao);
	fill_gpu_sprite_buffers();
	glBindVertexArray(Mesh::vao);
	fill_gpu_mesh_buffers();

	init_fonts();

	game_layer.init();
	
	// MAIN LOOP
	while(!glfwWindowShouldClose(window)) {
		
		double frame_start_time = glfwGetTime();

		// Call all GLFW callbacks
		glfwPollEvents();

		auto io = ImGui::GetIO();
		give_imgui_mouse_input();
		if (io.WantCaptureKeyboard || io.WantCaptureMouse) { fill_imgui_input(); } 
		else { game_input = global_input; }

		// Window resizing requests
		{
			if (global_input.was_pressed(GLFW_KEY_1)) { use_640_360(window); }
			if (global_input.was_pressed(GLFW_KEY_2)) { use_720p(window); }
			if (global_input.was_pressed(GLFW_KEY_3)) { use_1080p(window); }
		}

		glClear(GL_COLOR_BUFFER_BIT);  

		ImGui_ImplGlfwGL3_NewFrame();
		game_layer.update(seconds_per_update);
		game_layer.render();

		//ImGui::ShowDemoWindow();
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		global_input.reset_for_next_frame();

		// Wait until we hit the next frame time
		while (glfwGetTime() - frame_start_time < seconds_per_update) {}
	}
	glfwTerminate();
    return 0;
}
