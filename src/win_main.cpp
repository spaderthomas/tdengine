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

#include "imgui/imgui.h"
#include "imgui_impl_glfw_gl3.hpp"

// STL
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <experimental/filesystem>
#include <typeindex>
#include <optional>
#include <iomanip>
#include <limits>
using namespace std;

#include "machine_conf.hpp"
#include "log.hpp"
#include "db.hpp"
#include "script_to_def.hpp"
#include "utils.hpp"
#include "tdscript.hpp"
#include "text.hpp"
#include "sprite.hpp"
#include "texture.hpp"
#include "dialogue.hpp"
#include "task.hpp"
#include "battle.hpp"
#include "component.hpp"
#include "renderer.hpp"
#include "entity.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "level.hpp"
#include "game.hpp"
#include "collision.hpp"
#include "tdapi.hpp"
#include "glfw_callbacks.hpp"

#include "camera_impl.hpp"
#include "battle_impl.hpp"
#include "state.hpp"
#include "tdscript_impl.hpp"
#include "dialogue_impl.hpp"
#include "shader.hpp"
#include "transform.hpp"
#include "task_impl.hpp"
#include "sprite_impl.hpp"
#include "mesh.hpp"
#include "component_impl.hpp"
#include "draw.hpp"
#include "entity_impl.hpp"
#include "collision_impl.hpp"
#include "level_impl.hpp"
#include "renderer_impl.hpp"
#include "text_impl.hpp"
#include "game_impl.hpp"
#include "tdapi_impl.hpp"


int main() {
	tdns_log.init();
	
	auto result = glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	
	g_window = glfwCreateWindow((int)SCREEN_X, (int)SCREEN_Y, "tdengine", NULL, NULL);
	if (g_window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(g_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	glfwSetCursorPosCallback(g_window, GLFW_Cursor_Pos_Callback);
	glfwSetMouseButtonCallback(g_window, GLFW_Mouse_Button_Callback);
	glfwSetKeyCallback(g_window, GLFW_Key_Callback);
	glfwSetScrollCallback(g_window, GLFW_Scroll_Callback);
	glfwSetErrorCallback(GLFW_Error_Callback);
	
	glfwSwapInterval(0);

	component_pool.init();
	entity_pool.init();
	
	init_shaders();
	init_mesh();
	create_all_texture_atlas();
	create_texture("textures/misc/text_box.png");
	
	init_tdscript();
	change_window_size(tds_string("config", "screen", "default"));

	load_move_data();
					   
					   
	init_state();
	init_levels();
	init_fonts();
	
	init_hero();
	game.init();
	cutscene_thing.init();
	editor.init();
	battle.init();
	
	init_collider_matrix();
	
	test_tdscript();
	
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(g_window, false);
	auto& imio = ImGui::GetIO();
	imio.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();
	
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
	
	glGenVertexArrays(1, &Sprite::vao);
	glGenBuffers(1, &Sprite::vert_buffer);
	glGenBuffers(1, &Sprite::elem_buffer);
	glGenVertexArrays(1, &Mesh::vao);
	glGenBuffers(1, &Mesh::vert_buffer);
	glGenBuffers(1, &Mesh::elem_buffer);
	
	
	// Fill GPU sprite buffers
	glBindVertexArray(Sprite::vao);
	vector<float> vert_data;
	vector<float> tex_coords;
	
	concat(vert_data, square_verts);
	
	// Fill tex coordinate buffer
	for (auto asset : asset_table.assets) {
		Sprite* sprite = dynamic_cast<Sprite*>(asset);
		if (sprite) {
			sprite->tex_coord_offset = (GLvoid*)(sizeof(float) * vert_data.size());
			concat(vert_data, sprite->tex_coords);
		}
	}
	
	square_tex_coords_offset = (GLvoid*)(sizeof(float) * vert_data.size());
	concat(vert_data, square_tex_coords);
	
	// Send all the data to OpenGL buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, square_indices.size() * sizeof(uint), square_indices.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_data.size() * sizeof(float), vert_data.data(), GL_STATIC_DRAW);
	
	
	// Fill GPU mesh buffers
	glBindVertexArray(Mesh::vao);
	vector<Mesh*> all_meshes = asset_table.get_all<Mesh>();
	vector<float> vert_buffer;
	vector<uint> indx_buffer;
	
	// Collect all vertices and indices
	fox_for(imesh, all_meshes.size()) {
		Mesh* mesh = all_meshes[imesh];
		mesh->indices_offset = (GLvoid*)(sizeof(uint) * indx_buffer.size());
		indx_buffer.insert(indx_buffer.end(), mesh->indices.begin(), mesh->indices.end());
		mesh->vert_offset = (GLvoid*)(sizeof(float) * vert_buffer.size());
		vert_buffer.insert(vert_buffer.end(), mesh->verts.begin(), mesh->verts.end());
	}
	// Collect all tex coords
	fox_for(imesh, all_meshes.size()) {
		Mesh* mesh = all_meshes[imesh];
		mesh->tex_coord_offset = (GLvoid*)(sizeof(float) * vert_buffer.size());
		vert_buffer.insert(vert_buffer.end(), mesh->tex_coords.begin(), mesh->tex_coords.end());
	}
	
	// Give them to the OpenGL buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indx_buffer.size() * sizeof(uint), indx_buffer.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, Mesh::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_buffer.size() * sizeof(float), vert_buffer.data(), GL_STATIC_DRAW);
	
	
	// MAIN LOOP
	while(!glfwWindowShouldClose(g_window)) {
		double frame_start_time = glfwGetTime();
		
		// SETUP
		// Call all GLFW callbacks
		glfwPollEvents();
		
		// Pass all inputs to ImGui BEFORE ImGui::NewFrame
		auto io = ImGui::GetIO();
		give_imgui_mouse_input();
		if (io.WantCaptureKeyboard || io.WantCaptureMouse)
			fill_imgui_input();
		else
			active_layer->input = global_input;
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if (global_input.was_pressed(GLFW_KEY_F4)) {
			iactive_layer = (iactive_layer + 1) % all_layers.size();
			active_layer = all_layers[iactive_layer];
		}
		if (global_input.was_pressed(GLFW_KEY_F5)) {
			active_layer->reload();
		}
		
		// MEAT
		ImGui_ImplGlfwGL3_NewFrame();
		active_layer->update(seconds_per_update);
		active_layer->render();
		
		if (show_imgui_demo) { ImGui::ShowDemoWindow(); }
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(g_window);
		global_input.reset_for_next_frame();
		
		
		// Wait until we hit the next frame time
		if (print_framerate) {
			cout << (1 / (glfwGetTime() - frame_start_time)) << " fps\n";
		}
		while (glfwGetTime() - frame_start_time < seconds_per_update) {}
	}
    
	return 0;
}
