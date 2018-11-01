#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4624) // implicitly deleted constructor in component union

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

#include "sqlite/sqlite3.h"

#include "frozen/unordered_map.h"
#include "frozen/string.h"

// STL
#include <stdlib.h>
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
#include <optional>
#include <iomanip>
#include <limits>
using namespace std;


#include "imgui_impl_glfw_gl3.hpp"

#include "machine_conf.hpp"
#include "log.hpp"
#include "db.hpp"
#include "utils.hpp"
#include "input.hpp"
#include "text.hpp"
#include "asset_table.hpp"
#include "sprite.hpp"
#include "texture.hpp"
#include "animation.hpp"
#include "component.hpp"
#include "renderer.hpp"
#include "entity.hpp"
#include "level.hpp"
#include "game.hpp"
#include "tdns_lua.hpp"
#include "fsm.hpp"
#include "data.hpp"
#include "shader.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "sprite_impl.hpp"
#include "mesh.hpp"
#include "animation_impl.hpp"
#include "component_impl.hpp"
#include "entity_impl.hpp"
#include "box.hpp"
#include "draw.hpp"
#include "collision.hpp"
#include "level_impl.hpp"
#include "renderer_impl.hpp"
#include "text_impl.hpp"
#include "game_impl.hpp"
#include "testin.hpp"


sqlite3* db;

constexpr int const_strlen(const char* str) {
	int i = 0;
	while (str[i++]);
	return i;
}

/*
constexpr int row_to_index(const frozen::string table, const frozen::string row) {
	const auto& outer = db_schema.at(table);
	return outer.at(row);
}
*/
int main() {
	tdns_log.init();

	fox_assert(!sqlite3_open(db_dir.c_str(), &db));
	const char* sql_query =
		"select * from levels";
	const char* query_end = sql_query + strlen(sql_query);
	sqlite3_stmt* sql_statement;
	sqlite3_prepare(db, sql_query, 1024, &sql_statement, &query_end);
	sqlite3_step(sql_statement);
	const unsigned char* result = sqlite3_column_text(sql_statement, 0);
	sqlite3_close(db);

#pragma region GLFW_INIT
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	GLFWwindow* window = glfwCreateWindow((int)SCREEN_X, (int)SCREEN_Y, "tdengine", NULL, NULL);
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

	glfwSetCursorPosCallback(window, GLFW_Cursor_Pos_Callback);
	glfwSetMouseButtonCallback(window, GLFW_Mouse_Button_Callback);
	glfwSetKeyCallback(window, GLFW_Key_Callback);
	glfwSetScrollCallback(window, GLFW_Scroll_Callback);
	glfwSetErrorCallback(GLFW_Error_Callback);

	glfwSwapInterval(0);

	use_720p(window);
#pragma endregion 

#pragma region DATA_INIT
	component_pool.init();
	entity_pool.init();

	init_shaders();
	init_mesh();
	create_all_texture_atlas();
	create_texture("textures\\misc\\text_box.png");

	Lua.init();

	init_levels();

	init_fonts();

	game_layer.init();
#pragma endregion

#pragma region IMGUI_INIT
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, false);
	ImGui::StyleColorsDark();
#pragma endregion

#pragma region OPENGL_INIT
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
	#pragma endregion


	test();


	// MAIN LOOP
	while(!glfwWindowShouldClose(window)) {
		double frame_start_time = glfwGetTime();

		// SETUP
		// Call all GLFW callbacks
		glfwPollEvents();

		// Pass all inputs to ImGui BEFORE ImGui::NewFrame
		auto io = ImGui::GetIO();
		give_imgui_mouse_input();
		if (io.WantCaptureKeyboard || io.WantCaptureMouse) { fill_imgui_input(); } 
		else { game_input = global_input; }

		// Window resizing requests
		if (global_input.was_pressed(GLFW_KEY_F1)) { use_640_360(window); }
		if (global_input.was_pressed(GLFW_KEY_F2)) { use_720p(window); }
		if (global_input.was_pressed(GLFW_KEY_F3)) { use_1080p(window); }

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// MEAT
		ImGui_ImplGlfwGL3_NewFrame();
		game_layer.update(seconds_per_update);
		game_layer.render();
	
		if (show_imgui_demo) { ImGui::ShowDemoWindow(); }
		ImGui::End();
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		global_input.reset_for_next_frame();


		// Wait until we hit the next frame time
		if (print_framerate) {
			cout << (1 / (glfwGetTime() - frame_start_time)) << " fps\n";
		}
		while (glfwGetTime() - frame_start_time < seconds_per_update) {}
	}
    
	return 0;
}
