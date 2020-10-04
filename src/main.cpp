#include "libs.hpp"
#include "stl.hpp"

#include "machine_conf.hpp"
#include "log.hpp"
#include "utils.hpp"
#include "input.hpp"
#include "transform.hpp"
#include "console.hpp"
#include "tdlua.hpp"
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
#include "lua_impl.hpp"
#include "physics_impl.hpp"
#include "shader_impl.hpp"
#include "transform_impl.hpp"

int main() {
	tdns_log.init();
	init_path_constants();

	EXIT_IF_ERROR(init_glfw());
	init_imgui();

	auto& asset_manager  = get_asset_manager();
	auto& input_manager  = get_input_manager();
	auto& render_engine  = get_render_engine();
	auto& entity_manager = get_entity_manager();
	auto& physics_engine = get_physics_engine();
	
	init_shaders();
	init_mesh();
	init_assets();

	Lua.init();

	init_fonts();
	
	init_gl();

	// @spader 9/4/2020: When the game gets much farther along, this isn't how you kick off entity creation
	entity_manager.create_entity("Editor");

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

		// MEAT
		ImGui_ImplGlfwGL3_NewFrame();
		if (show_imgui_demo) { ImGui::ShowDemoWindow(); }
		if (show_console) { console.Draw("tdengine"); }
		
		entity_manager.update(seconds_per_update);
		physics_engine.update(seconds_per_update);

		Render_Element r;
		r.flags = Render_Flags::None;
		r.layer = 0;
		r.world_pos[0] = 0;
		r.world_pos[1] = 0;
		r.sprite = asset_manager.get_asset<Sprite>("classroom");
		render_engine.render_list.push_back(r);

		render_engine.render();

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(g_window);
		input_manager.end_frame();
		
		
		// Wait until we hit the next frame time
		framerate = 1.f / (glfwGetTime() - frame_start_time);
		Lua.state["tdengine"]["framerate"] = framerate;
		
		while (glfwGetTime() - frame_start_time < seconds_per_update) {}
	}

	return 0;
}
