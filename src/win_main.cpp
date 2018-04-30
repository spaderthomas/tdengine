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
#include <string>
#include <algorithm>
using namespace std;

#include "utils.hpp"
#include "log.hpp"
#include "data.hpp"
#include "asset_table.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "animation.hpp"
#include "transform.hpp"
#include "entity.hpp"
#include "entity_table.hpp"
#include "asset_table_functions.hpp"

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

	
	// ENTITY INIT
	glGenVertexArrays(1, &Entity::vao);
	glGenBuffers(1, &Entity::vert_buffer);
	glGenBuffers(1, &Entity::elem_buffer);
	
	glBindVertexArray(Entity::vao);
	fill_gpu_sprite_buffers();
	
	Texture* wall_texture = asset_table.get_texture("wall_tex");
	Texture* octopus_texture = asset_table.get_texture("octopus_tex");
	wall_texture->init("../../textures/wall.jpg");
	octopus_texture->init("../../textures/octopus.png");
	
	Entity* wall = entity_table.get_entity("wall");
	Entity* octopus = entity_table.get_entity("octopus");

	Animation* wall_anim = asset_table.get_animation("wall_anim");
	wall_anim->add_frame("wall_tex");

	Entity* ball = entity_table.get_entity("ball");

	Texture* ball_anim1 = asset_table.get_texture("ball1");
	Texture* ball_anim2 = asset_table.get_texture("ball2");
	Texture* ball_anim3 = asset_table.get_texture("ball3");
	Texture* ball_anim4 = asset_table.get_texture("ball4");
	Texture* ball_anim5 = asset_table.get_texture("ball5");
	ball_anim1->init("../../textures/animation_test1.png");
	ball_anim2->init("../../textures/animation_test2.png");
	ball_anim3->init("../../textures/animation_test3.png");
	ball_anim4->init("../../textures/animation_test4.png");
	ball_anim5->init("../../textures/animation_test5.png");
	
	Animation* ball_anim = asset_table.get_animation("ball_anim");
	ball_anim->add_frame(ball_anim1);
	ball_anim->add_frame(ball_anim2);
	ball_anim->add_frame(ball_anim3);
	ball_anim->add_frame(ball_anim4);
	ball_anim->add_frame(ball_anim5);

	ball->animation_ids.push_back("ball_anim");
	ball->start_animation("ball_anim");

	SRT transform;
	transform.translate = glm::vec2(2.3, 3.1);
	mat3_from_transform(transform);
	
	// MAIN LOOP
	float spf = 1.f / 10.f;
	while(!glfwWindowShouldClose(window)) {
		double frame_start_time = glfwGetTime();
		process_input(window);

		// Note: Hannah's favorite three floating point numbers. Do not remove!
		glClearColor(.82f, .77f, 0.57f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Render
		basic_shader.bind();
		ball->bind();
		glUniform1i(basic_shader.get_uniform_loc("sampler1"), 0);
		ball->draw(GL_TRIANGLES);
			
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Wait until we hit the next frame time
		while (glfwGetTime() - frame_start_time < spf) {}
	}
	glfwTerminate();
    return 0;
}
