#include "libs.hpp"

#include "machine_conf.hpp"
#include "log.hpp"
#include "utils.hpp"
#include "input.hpp"
#include "transform.hpp"
#include "console.hpp"
#include "lua.hpp"
#include "asset.hpp"
#include "font.hpp"
#include "entity.hpp"
#include "draw.hpp"
#include "shader.hpp"
#include "physics.hpp"
#include "imgui/imgui_lua_bindings.hpp"
#include "api.hpp"

#include "api_impl.hpp"
#include "asset_impl.hpp"
#include "console_impl.hpp"
#include "draw_impl.hpp"
#include "entity_impl.hpp"
#include "font_impl.hpp"
#include "lua_impl.hpp"
#include "physics_impl.hpp"
#include "shader_impl.hpp"
#include "transform_impl.hpp"

int main() {
	tdns_log.init();

	EXIT_IF_ERROR(init_glfw());
	init_imgui();

	// These are all function static globals -- access them now to ensure they exist
	// (even though you shouldn't have to) and to initialize them
	auto& asset_manager      = get_asset_manager();
	auto& input_manager      = get_input_manager();
	auto& shader_manager     = get_shader_manager();
	auto& render_engine      = get_render_engine();
	auto& entity_manager     = get_entity_manager();
	auto& component_manager  = get_component_manager();
	auto& cutscene_manager   = get_cutscene_manager();
	auto& physics_engine     = get_physics_engine();
	auto& interaction_system = get_interaction_system();
	auto& update_system      = get_update_system();

	render_engine.init();

	init_shaders();
	init_mesh();
	init_assets();
	init_fonts();
	init_gl();
	Lua.init();

	// MAIN LOOP
	while(!glfwWindowShouldClose(g_window)) {
		double frame_start_time = glfwGetTime();

		file_watcher.update();

		if (send_kill_signal) return 0;
		
		// SETUP
		// Call all GLFW callbacks
		glfwPollEvents();

		bool run_update = true;
		run_update = !step_mode || input_manager.was_pressed(GLFW_KEY_SPACE);
		step_mode = step_mode && !input_manager.was_pressed(GLFW_KEY_ENTER);

		if (run_update) {
			// These have to happen before ImGui::NewFrame
			fill_imgui_input();
			load_imgui_layout();
		
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// MEAT
			ImGui_ImplGlfwGL3_NewFrame();
			if (show_imgui_demo) { ImGui::ShowDemoWindow(); }
			if (show_console) { console.Draw("tdengine"); }
		
			entity_manager.update(seconds_per_update);
			cutscene_manager.update(seconds_per_update);
			physics_engine.update(seconds_per_update);
			interaction_system.update(seconds_per_update);
			update_system.update(seconds_per_update);

			render_engine.render(seconds_per_update);

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		
			glfwSwapBuffers(g_window);
		}

		// The input manager needs to still work so we can do stuff in step mode
		input_manager.end_frame();

		if (run_update) {
			// Wait until we hit the next frame time
			framerate = 1.f / (glfwGetTime() - frame_start_time);
			Lua.state["tdengine"]["framerate"] = framerate;
			
			while (glfwGetTime() - frame_start_time < seconds_per_update) {}
		}
	}

	return 0;
}
