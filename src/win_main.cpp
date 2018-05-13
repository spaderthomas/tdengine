#define _CRT_SECURE_NO_WARNINGS

// Library includes
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb/stb_rect_pack.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// STL
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cmath>
#include <experimental/filesystem>
using namespace std;

#include "utils.hpp"
#include "log.hpp"
#include "data.hpp"
#include "asset_table.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "sprite.hpp"
#include "texture_atlas.hpp"
#include "mesh.hpp"
#include "transform.hpp"
#include "renderer.hpp"
#include "animation.hpp"
#include "entity.hpp"
#include "entity_table.hpp"
#include "asset_table_functions.hpp"
#include "input.hpp"
#include "assets.hpp"
#include "draw.hpp"
#include "player.hpp"
#include "renderer_functions.hpp"
#include "game.hpp"


int main() {
	tdns_log.init();
	
	// GLFW INIT
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);

	init_assets();

	// OPENGL INIT
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

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


	// GAME INIT
	game_layer.init();

	// MAIN LOOP
	while(!glfwWindowShouldClose(window)) {
		double frame_start_time = glfwGetTime();

		// This will call all of our callbacks, giving us all the input for this frame
		glfwPollEvents();
		if (game_input_active) { copy_input(global_input, game_input); }

		if (global_input.was_pressed(TDNS_KEY_1)) { use_640_360(window); }
		if (global_input.was_pressed(TDNS_KEY_2)) { use_720p(window); }
		if (global_input.was_pressed(TDNS_KEY_3)) { use_1080p(window); }

		// Note: Hannah's favorite three floating point numbers. Do not remove!
		glClearColor(.82f, .77f, 0.57f, 1.0f);
		if (global_input.is_down[TDNS_MOUSE_LEFT]) {
			glClearColor(0.f, .77f, 0.57f, 1.0f);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
		
		game_layer.update(seconds_per_update);
		game_layer.render();

		glfwSwapBuffers(window);
		global_input.reset_for_next_frame();


		// Wait until we hit the next frame time
		while (glfwGetTime() - frame_start_time < seconds_per_update) {}
	}
	glfwTerminate();
    return 0;
}
