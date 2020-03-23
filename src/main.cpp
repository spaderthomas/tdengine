#include "libs.hpp"
#include "stl.hpp"

#include "machine_conf.hpp"
#include "log.hpp"
#include "utils.hpp"
#include "input.hpp"
#include "transform.hpp"
#include "tdlua.hpp"
#include "asset.hpp"
#include "font.hpp"
#include "draw.hpp"
#include "shader.hpp"
#include "entity.hpp"
#include "console.hpp"
#include "imgui/imgui_lua_bindings.hpp"
#include "api.hpp"

#include "api_impl.hpp"
#include "asset_impl.hpp"
#include "console_impl.hpp"
#include "draw_impl.hpp"
#include "entity_impl.hpp"
#include "lua_impl.hpp"
#include "shader_impl.hpp"
#include "transform_impl.hpp"

//#include "component.hpp"
//#include "dialogue.hpp"
//#include "task.hpp"
//#include "actions/action_includes.hpp"
//#include "battle.hpp"
//#include "components/component_includes.hpp"
//#include "renderer.hpp"
//#include "entity.hpp"
//#include "camera.hpp"
//#include "state.hpp"
//#include "layer.hpp"
//#include "level.hpp"
//#include "game.hpp"
//#include "editor.hpp"
//#include "layers.hpp"
//#include "collision.hpp"
//#include "tdapi.hpp"
//#include "camera_impl.hpp"
//#include "battle_impl.hpp"
//#include "state_impl.hpp"
//#include "tdscript_impl.hpp"
//#include "dialogue_impl.hpp"
//#include "actions/action_impl_includes.hpp"
//#include "task_impl.hpp"
//#include "components/component_impl_includes.hpp"
//#include "entity_impl.hpp"
//#include "collision_impl.hpp"
//#include "level_impl.hpp"
//#include "renderer_impl.hpp"
//#include "editor_impl.hpp"
//#include "game_impl.hpp"
//#include "tdapi_impl.hpp"
// @CANARY

int main() {
	tdns_log.init();
	init_path_constants();

	EXIT_IF_ERROR(init_glfw());
	init_imgui();
	
	//component_pool.init();
	//entity_pool.init();

	init_shaders();
	init_mesh();
	create_all_texture_atlas();

	Lua.init();
	Lua.test();
	//init_tdscript();
	//change_window_size(tds_string("config", "screen", "default"));

	// Load all the game data out of TDS files
	//init_moves();
	//init_state();
	//init_levels();
	init_fonts();
	//init_cutscenes();
	//init_hero();
	//template_components.init();
	
	//game.init();
	//editor.init();
	//battle.init();
	
	init_collider_matrix();
	
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
	std::vector<float> vert_data;
	std::vector<float> tex_coords;
	
	concat(vert_data, square_verts);
	
	// Fill tex coordinate buffer
	auto& asset_manager = get_asset_manager();
	auto sprites = asset_manager.get_all<Sprite>();
	for (auto sprite : sprites) {
		sprite->tex_coord_offset = (GLvoid*)(sizeof(float) * vert_data.size());
		concat(vert_data, sprite->tex_coords);
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
	std::vector<Mesh*> all_meshes = asset_manager.get_all<Mesh>();
	std::vector<float> vert_buffer;
	std::vector<uint> indx_buffer;
	
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
	

	auto imgui_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(get_default_font_path().c_str(), 16.0);
	ImGui::GetIO().IniFilename = RelativePath("imgui.ini").path.c_str();
   
	// MAIN LOOP
	while(!glfwWindowShouldClose(g_window)) {
		double frame_start_time = glfwGetTime();

		file_watcher.update();

		if (send_kill_signal) return 0;
		
		// SETUP
		// Call all GLFW callbacks
		glfwPollEvents();
		
		// Pass all inputs to ImGui BEFORE ImGui::NewFrame
		fill_imgui_input();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto& input_manager = get_input_manager();
		if (input_manager.was_pressed(GLFW_KEY_LEFT_CONTROL)) {
			show_console = !show_console;
		}
		
		// MEAT
		ImGui_ImplGlfwGL3_NewFrame();
		if (show_imgui_demo) { ImGui::ShowDemoWindow(); }
		
		Lua.update(seconds_per_update);

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(g_window);
		input_manager.end_frame();
		
		
		// Wait until we hit the next frame time
		if (print_framerate) {
			std::cout << (1 / (glfwGetTime() - frame_start_time)) << " fps\n";
		}
		while (glfwGetTime() - frame_start_time < seconds_per_update) {}
	}

	return 0;
}
