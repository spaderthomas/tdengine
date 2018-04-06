#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <fstream>

using namespace std;

#define fox_for(iterName, iterCount) for (unsigned int iterName = 0; iterName < (iterCount); ++iterName)

// Functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}



void gl_unbind_buffer() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// In-program shapes
float triangle[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

float screen[] = {
	1.f, 1.f, 0.f,
    -1.f, -1.f, 0.f,
	-1.f, 1.f, 0.f,
	1.f, -1.f, 0.f,
};

struct Shader {
	unsigned int id;
	
	void check_gl_error(GLint shader, GLint kind) {
		int success;
		char info_log[512];
		glGetShaderiv(shader, kind, &success);
		if(!success) {
			glGetShaderInfoLog(shader, 512, NULL, info_log);
			ofstream log("log.txt");
			log.write(info_log, 512);
			exit(1);
		}
	}
	
	void init(const char* vs_path, const char* fs_path) {
		const char* paths[] = {
			vs_path,
			fs_path
		};

		unsigned int shader_program;
		shader_program = glCreateProgram();
		
		fox_for(ishader, 2) {
			// Read in shader data
			const char* path = paths[ishader];
			FILE *shader_source_file = fopen(path, "rb");
			fseek(shader_source_file, 0, SEEK_END);
			unsigned int fsize = ftell(shader_source_file);
			fseek(shader_source_file, 0, SEEK_SET);

			char *source = (char*)malloc(fsize + 1);
			fread(source, fsize, 1, shader_source_file);
			source[fsize] = 0;
			fclose(shader_source_file);

			// Compile the shader
			unsigned int shader_kind = (ishader == 0) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
			unsigned int shader = glCreateShader(shader_kind);
			glShaderSource(shader, 1, &source, NULL);
            glCompileShader(shader);
            check_gl_error(shader, GL_COMPILE_STATUS);
            glAttachShader(shader_program, shader);

		}
		
		// Link into a shader program
		glLinkProgram(shader_program);
		check_gl_error(shader_program, GL_LINK_STATUS);
		id = shader_program;
	}

	unsigned int get_uniform_loc(const char* name) {
		return glGetUniformLocation(id, name);
	}

	void bind() {
		glUseProgram(id);
	}
};

int main() {
	// GLFW
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "demo", NULL, NULL);
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
	
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// Get shader text
    Shader basic_shader;
    basic_shader.init("../shaders/basic.vs", "../shaders/basic.fs");
	
	// Buffer setup
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
	
	// Tell OpenGL how our data is formatted
	glVertexAttribPointer(0,                  // index
						  3,                  // vertices per polygon
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalize it?
						  3 * sizeof(float),  // stride between X's (wlog)
						  (void*)0);          // offset to first
	glEnableVertexAttribArray(0);
	gl_unbind_buffer();

	int x_offset = 0;
	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Render
		basic_shader.bind();
		float time = glfwGetTime();
		float greenness = (sin(time) / 2.f) + .5f;
		unsigned int color_loc = basic_shader.get_uniform_loc("color");
		glUniform4f(color_loc, 0.f, greenness, 0.f, 1.f);
		unsigned int offset_loc = basic_shader.get_uniform_loc("x_offset");
		glUniform1f(offset_loc, sin(x_offset));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		process_input(window);

		x_offset++;
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
    return 0;
}
 
