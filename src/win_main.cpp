#define _CRT_SECURE_NO_WARNINGS

// Library includes
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// STL
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "utils.hpp"
#include "log.hpp"
#include "data.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "sprite.hpp"

int main() {
	tdns_log.init();
	
	// GLFW INIT
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(320, 240, "demo", NULL, NULL);
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
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	
	// SHADER INIT
    Shader basic_shader;
    basic_shader.init("../../shaders/basic.vs", "../../shaders/basic.fs");

	
	// SPRITE INIT
	glGenVertexArrays(1, &Sprite::vao);
	glGenBuffers(1, &Sprite::vert_buffer);
	glGenBuffers(1, &Sprite::elem_buffer);
	
	glBindVertexArray(Sprite::vao);
	fill_gpu_sprite_buffers();
	
	Texture wall_texture, octopus_texture;
	wall_texture.init("../../textures/wall.jpg");
	octopus_texture.init("../../textures/octopus.png");
	
   	Sprite wall, octopus;
	wall.texture = wall_texture;
	octopus.texture = octopus_texture;


	// MAIN LOOP
	while(!glfwWindowShouldClose(window)) {
		process_input(window);
		
		// Note: Hannah's favorite three floating point numbers. Do not remove!
		glClearColor(.82f, .77f, 0.57f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Render
		basic_shader.bind();
		octopus.bind();
		glUniform1i(basic_shader.get_uniform_loc("sampler1"), 0);
		octopus.draw(GL_TRIANGLES);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
    return 0;
}
