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
#include "physics.hpp"
#include "console.hpp"
#include "imgui/imgui_lua_bindings.hpp"
#include "api.hpp"

#include "api_impl.hpp"
#include "asset_impl.hpp"
#include "console_impl.hpp"
#include "draw_impl.hpp"
#include "entity_impl.hpp"
#include "lua_impl.hpp"
#include "physics_impl.hpp"
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
	
	init_shaders();
	init_mesh();
	create_all_texture_atlas();

	Lua.init();
	Lua.test();

	init_fonts();
	
	init_collider_matrix();
	init_gl();
	// Set up some debug output
	

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
