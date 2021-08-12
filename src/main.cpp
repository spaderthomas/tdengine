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
	init_lua();

	// MAIN LOOP
	while(!glfwWindowShouldClose(g_window)) {
		double frame_start_time = glfwGetTime();
		
		if (send_kill_signal) return 0;

		file_watcher.update();
		
		glfwPollEvents();

		bool run_update = true;
		run_update &= !are_updates_paused;
		run_update &= !step_mode || input_manager.was_pressed(GLFW_KEY_SPACE);
		step_mode  &= !input_manager.was_pressed(GLFW_KEY_ENTER);

		fill_imgui_input();
		load_imgui_layout();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplGlfwGL3_NewFrame();
		
		if (show_imgui_demo) ImGui::ShowDemoWindow();
		if (show_console) console.Draw("tdengine");

		// Update all the systems
		if (run_update) {
			update_system.run_entity_updates(seconds_per_update);
			cutscene_manager.update(seconds_per_update);
			interaction_system.update(seconds_per_update);

			// First, run the physics engine to at least let attached entities submit requests to
			// sync to what they're attached to.
			physics_engine.update(seconds_per_update);
			update_system.run_collision_callbacks(seconds_per_update);

			// Then, keep running it while positions aren't fully resolved.
			while (!physics_engine.requests.empty()) {
				physics_engine.update(seconds_per_update);
				update_system.run_collision_callbacks(seconds_per_update);
			}
			
			update_system.run_interaction_callback(seconds_per_update);

		} else {
			update_system.do_paused_update(seconds_per_update);
		}

		// Render 
		render_engine.render(seconds_per_update);
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(g_window);

		// Clean up the frame
		entity_manager.destroy_flagged_entities();

		input_manager.end_frame();

		framerate = 1.f / (glfwGetTime() - frame_start_time);
		Lua.state["tdengine"]["framerate"] = framerate;
		Lua.state["tdengine"]["input_mask"] = input_manager.mask;
		Lua.state["tdengine"]["old_input_mask"] = input_manager.old_mask;
		while (glfwGetTime() - frame_start_time < seconds_per_update) {}
	}

	return 0;
}
