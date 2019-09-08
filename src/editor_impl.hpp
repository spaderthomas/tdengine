Entity_Tree* Entity_Tree::create(string dir) {
	Entity_Tree* tree = new Entity_Tree;
	tree->dir = name_from_full_path(dir);
	for (auto it = directory_iterator(dir); it != directory_iterator(); ++it) {
		string path = it->path().string();
		if (is_regular_file(it->status())) {
			if (is_png(path)) {
				string name = strip_extension(name_from_full_path(path));
				tree->entities.push_back(Entity::create(name));
				tree->size++;
			}
		}
		else if (is_directory(it->status())) {
			tree->children.push_back(create(path));
		}
	}
	
	return tree;
}
pool_handle<Entity> Entity_Tree::find(string name) {
	for (auto& handle : entities) {
		if (handle()->name == name) {
			return handle;
		}
	}
	for (auto& child : children) {
		pool_handle<Entity> found_in_child = child->find(name);
		if (found_in_child != -1) {
			return found_in_child;
		}
	}
	
	return { -1, nullptr };
}

void Editor::init() {
	active_level = levels["overworld"];
	tile_tree = Entity_Tree::create(absolute_path("textures/tiles"));
}
void Editor::translate() {
	get_cmp(selected, Position_Component)->world_pos =
		snap_to_grid ?
		screen_from_grid(grid_from_world(input.world_pos)) :
	input.world_pos + smooth_drag_offset;
}
void Editor::delete_selected() {
	// Kill the thing from the level 
	for (auto it = active_level->entities.begin(); it != active_level->entities.end(); it++) {
		if (selected == *it) {
			selected->clear_components();
			active_level->entities.erase(it);
			break;
		}
	}
	
	// Kill the thing from the editor
	selected = { -1, nullptr };
}

void Editor::draw_component_editor() {
	ImGui::Begin("components", 0, ImGuiWindowFlags_AlwaysAutoResize);
	
	// Iterate through components, displaying whatever you need
	for (auto& kvp : selected()->components) {
		pool_handle<any_component> handle = kvp.second;
		Component* component = (Component*)handle();
		
		//@metaprogramming
		if (dynamic_cast<Graphic_Component*>(component)) {
			if (ImGui::TreeNode("Graphic Component")) {
				def_cast_cmp(gc, component, Graphic_Component);
				
				// Display animation info
				Animation* current_animation = gc->active_animation;
				if (ImGui::BeginCombo("Animations", current_animation->name.c_str(), 0)) {
					for (auto anim : gc->animations) {
						bool is_selected = anim->name == current_animation->name;
						if (ImGui::Selectable(anim->name.c_str(), is_selected)) {
							set_animation(selected, anim->name);
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				
				// Display scale info
				ImGui::SliderFloat2("Scale", glm::value_ptr(gc->scale), 0.f, 1.f);
				ImGui::TreePop();
			}
		}
		else if (dynamic_cast<Door_Component*>(component)) {
			if (ImGui::TreeNode("Door Component")) {
				def_cast_cmp(door, component, Door_Component);
				if (ImGui::BeginCombo("##setdoor", door->level.c_str(), 0)) {
					for (auto& kvp : levels) {
						const string& name = kvp.first;
						bool is_selected = door->level.c_str() == name;
						if (ImGui::Selectable(name.c_str(), &is_selected)) {
							door->level = name;
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::TreePop();
			}
		}
		else if (dynamic_cast<Position_Component*>(component)) {
			if (ImGui::TreeNode("Position Component")) {
				def_cast_cmp(pc, component, Position_Component);
				ImGui::SliderFloat2("Position", glm::value_ptr(pc->world_pos), 0.f, 2.f);
				ImGui::TreePop();
			}
		}
		else if (dynamic_cast<BattleComponent*>(component)) {
			if (ImGui::TreeNode("Battle Component")) {
				def_cast_cmp(bc, component, BattleComponent);
				ImGui::SliderInt("Health", (int*)&bc->health, 0, 10);
				ImGui::TreePop();
			}
		}
		else if (dynamic_cast<TileComponent*>(component)) {
			if (ImGui::TreeNode("Tile Component")) {
				def_cast_cmp(tc, component, TileComponent);
				int min = tc->x - 20;
				int max = tc->x + 20;
				ImGui::SliderInt("Position", &tc->x, min, max);
				ImGui::TreePop();
			}
		}
	}
	
	ImGui::End();
}
void Editor::draw_tile_tree(Entity_Tree* root) {
	if (ImGui::CollapsingHeader("Tile Selector")) {
		for (auto child : root->children) {
			draw_tile_tree_recursive(child, 0);
		}
	}
}
int Editor::draw_tile_tree_recursive(Entity_Tree* root, int unique_btn_index) { 
	// If expanded, draw this folder's tiles
	if (ImGui::TreeNode(root->dir.c_str())) {
		fox_for(i, root->entities.size()) {
			Entity* tile = root->entities[i](); // no pool handle?
			Graphic_Component* gc = tile->get_component<Graphic_Component>();
			if (gc) {
				Sprite* ent_sprite = gc->get_current_frame();
				
				ImVec2 top_right_tex_coords = ImVec2(ent_sprite->tex_coords[2], ent_sprite->tex_coords[3]);
				ImVec2 bottom_left_tex_coords = ImVec2(ent_sprite->tex_coords[6], ent_sprite->tex_coords[7]);
				ImVec2 button_size = ImVec2(32, 32);
				ImGui::PushID(unique_btn_index++);
				if (ImGui::ImageButton((ImTextureID)ent_sprite->atlas->handle,
									   button_size,
									   bottom_left_tex_coords, top_right_tex_coords))
				{
					selected = Entity::create(tile->name);
					this->kind = TILE;
					this->state = INSERT;
					
					// We've selected a tile, so enable the grid and save the old preferences
					last_show_grid = show_grid;
					last_snap_to_grid = snap_to_grid;
					show_grid = true;
					snap_to_grid = true;
				}
				ImGui::PopID();
				bool is_end_of_row = !((i + 1) % 6);
				bool is_last_element = i == (root->size - 1);
				if (!is_end_of_row && !is_last_element) { ImGui::SameLine(); }
			}
		}
		
		// And draw dropdowns for any children
		for (auto child : root->children) {
			unique_btn_index = draw_tile_tree_recursive(child, unique_btn_index);
		}
		
		ImGui::TreePop();
	}
	
	return unique_btn_index;
}
void Editor::exec_console_cmd(char* command_line) {
	char* command = strtok(command_line, " ");
	if (!command) return;
	
	
	if (console.Stricmp(command, "save") == 0) {
		active_level->save();
	}
	else if (console.Stricmp(command, "go") == 0) {
		char* x = strtok(NULL, " ");
		if (!x) {
			console.AddLog("format: go [x: float] [y: float]");
			return;
		}
		
		char* y = strtok(NULL, " ");
		if (!y) {
			console.AddLog("format: go [x: float] [y: float]");
			return;
		}
		
		if (!is_float(x) || !is_float(y)) {
			console.AddLog("format: go [x: float] [y: float]");
			return;
		}
		
		camera.offset = glm::vec2{stof(x), stof(y)};
	}
	else if (console.Stricmp(command, "load") == 0) {
		active_level->load();
	}
	else if (console.Stricmp(command, "grid") == 0) {
		if (kind == TILE && state != IDLE) {
			last_show_grid = !show_grid;
		} else {
			last_show_grid = show_grid;
		}
		show_grid = !show_grid;
	}
	else if (console.Stricmp(command, "snap") == 0) {
		// If we're actually in tile mode, assume we want this new change to persist when we go back
		if (kind == TILE && state != IDLE) {
			last_snap_to_grid = !snap_to_grid;
		} else {
			last_snap_to_grid = snap_to_grid;
		}
		snap_to_grid = !snap_to_grid;
		
	}
	else if (console.Stricmp(command, "level") == 0) {
		swap_level(this, strtok(NULL, " "));
	}
	else {
		console.AddLog("Unknown command: '%s'. Loading up Celery Man instead.\n", command_line);
	}
}
void Editor::reload_assets() {
	// @leak
	create_all_texture_atlas();
	tile_tree = Entity_Tree::create(absolute_path("textures/tiles"));
	active_level->load();
}
void Editor::reload() {
	// Reset the script state; anything unsaved will be lost
	init_tdscript(); // @leak
	init_state();
	init_levels();     // @leak
	
	init_hero();
	game.init();
	editor.init();
	battle.init();
	
	camera.following = NULL_ENTITY;
	reload_assets();
}
void Editor::undo_action() {
	if (action_stack.size()) {
		function<void()> fn = action_stack.back();
		action_stack.pop_back();
		fn();
	}
}
void Editor::undo_mark() {
	if (mark_stack.size()) {
		function<void()> fn = mark_stack.back();
		mark_stack.pop_back();
		fn();
	}
}
void Editor::update(float dt) {	
	static bool open = true;
	ShowExampleAppCustomNodeGraph(&open);
		
	if (input.is_down[GLFW_KEY_W]) {
		camera.offset += glm::vec2{0, .025};
	}
	if (input.is_down[GLFW_KEY_S]) {
		camera.offset += glm::vec2{0, -.025};
	}
	if (input.is_down[GLFW_KEY_A]) {
		camera.offset += glm::vec2{-.025, 0};
	}
	if (input.is_down[GLFW_KEY_D]) {
		camera.offset += glm::vec2{.025, 0};
	}
	
	// Global ESC -- puts you back in idle
	if (input.was_pressed(GLFW_KEY_ESCAPE)) {
		selected = { -1, nullptr };
		state = IDLE;
		
		// If you had a tile selected, put the grid settings back to how they were
		if (kind == TILE) {
			snap_to_grid = last_snap_to_grid;
			show_grid = last_show_grid;
		}
	}
	if (input.is_down[GLFW_KEY_LEFT_ALT] &&
		input.was_pressed(GLFW_KEY_Z)) {
		undo_action();
	}
	
	// Toggle the console on control
	if (show_console) {
		console.Draw("tdconsole");
	}
	
	
	// GUI
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "save level")) { 
				active_level->save(); 
			}
			if (ImGui::MenuItem("Load", "load level")) { 
				active_level->load(); 
			}
			if (ImGui::BeginMenu("Reload", "reload")) {
				if (ImGui::MenuItem("assets")) {
					reload_assets();
				}
				if (ImGui::MenuItem("everything")) {
					reload();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Options", "options")) {
				ImGui::MenuItem("Show Bounding Boxes", "", &debug_show_aabb);
				ImGui::MenuItem("Show Minkowski Differences", "", &debug_show_minkowski);
				ImGui::MenuItem("Show ImGui Demo", "", &show_imgui_demo);
				ImGui::MenuItem("Show Framerate", "", &print_framerate);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Windows", "windows")) {
				ImGui::MenuItem("Tile Selector", "", &show_tile_selector);
				ImGui::MenuItem("Script Selector", "", &show_script_selector);
				ImGui::MenuItem("Level Selector", "", &show_level_selector);
				ImGui::MenuItem("State Tweaker", "", &show_state_tweaker);
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "M-z")) { undo_action(); }
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	
	// Global for debug display
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
	
	if (show_tile_selector) {
		ImGui::Begin("Tiles", 0, flags);
		draw_tile_tree(tile_tree);
		ImGui::End();
	}
	
	if (show_task_editor) {
		static bool init = false;
		static TaskEditorNode* task_graph = nullptr;
		
		// Pull in a random task 
		if (!init) {
			Task* test = new Task;
			TableNode* table = tds_table2(ScriptManager.global_scope, "entity", "wilson", "scripts", "intro");
			test->init_from_table(table, { -1, nullptr });
			task_editor.task_graph = make_task_graph(test, ImVec2(200, 200));
			init = true;
		}
		
		task_editor.show();
	}
	
	if (show_script_selector) {
		ImGui::Begin("Scripts", 0, flags);
		static ImGuiTextFilter filter;
		filter.Draw("Filter");
		
		static string script_current = script_to_entity.begin()->first;
		
		// Note: I'm not entirely sure why this name has to include ##
		// It certainly has something to do with the ImGui stack, but I thought
		// that only mattered in the case of duplicate names in the same window
		if (ImGui::BeginCombo("##choose_script", script_current.c_str(), 0)) {
			for (auto& kvp : script_to_entity) {
				auto& script = kvp.first;
				if (filter.PassFilter(script.c_str())) {
					bool is_selected = (script == script_current);
					if (ImGui::Selectable(script.c_str(), is_selected)) {
						script_current = script;
					}
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		
		// Populate a list of selectables that, when clicked, create the corresponding entity
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysAutoResize);
		for (auto& entity : script_to_entity[script_current]) {
			if (ImGui::Selectable(entity.c_str())) {
				// If we were selecting a tile, reset our grid settings
				if (this->kind == TILE) {
					snap_to_grid = last_snap_to_grid;
					show_grid = last_show_grid;
				}
				
				// Create the entity and go to the new editor state
				selected = Entity::create(entity);
				this->kind = ENTITY;
				this->state = INSERT;
				
				
			}
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::End();
	}
	
	if (show_level_selector) {
		ImGui::Begin("Levels", 0, flags);
		static string level_current = active_level->name;
		if (ImGui::BeginCombo("##chooselevel", level_current.c_str(), 0)) {
			fox_iter(it, levels) {
				string level_name = it->first;
				bool is_selected = level_name == level_current;
				if (ImGui::Selectable(level_name.c_str(), is_selected)) {
					level_current = level_name;
					active_level = it->second;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::End();
	}
	
	if (show_state_tweaker) {
		ImGui::Begin("State", 0, flags);
		for (auto& kvp : game_state) {
			// Red if it's false, green if it's true
			bool value = kvp.second;
			if (value) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 255.f, 0.f, 255.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 255.f, 255.f));
			} else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(255.f, 0.f, 0.f, 255.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 255.f, 255.f));
			}
			
			// If we press it, toggle the state
			if (ImGui::Button(kvp.first.c_str())) {
				update_state(kvp.first, !value);
				cout << "vbdafa";
			}
			
			ImGui::PopStyleColor(2);
		}
		ImGui::End();
	}
	
	
	// Editing logic
	if (state == IDLE) {
		if (input.was_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
			bool clicked_inside_something = false;
			for (auto& handle : active_level->entities) {
				Entity* entity = handle();
				auto box = Center_Box::from_entity(handle);
				if (box) {
					if (point_inside_box(input.world_pos, *box)) {
						clicked_inside_something = true;
						selected = handle;
						smooth_drag_offset = entity->get_component<Position_Component>()->world_pos - input.world_pos; // So we don't jump to the exact mouse position
						state = DRAG;
						break;
					}
				}
			}
#if 0
			if (!clicked_inside_something) {
				top_left_drag = input.screen_pos;
				editor_state = RECTANGLE_SELECT;
			}
#endif
		}
		
	}
	else if (state == INSERT) {
		translate();
		
		// And if we click, add it to the level
		if (input.is_down[GLFW_MOUSE_BUTTON_LEFT]) {
			if (kind == TILE) {
				auto grid_pos = grid_from_world(input.world_pos);
				pool_handle<Entity> handle = active_level->get_tile(grid_pos.x, grid_pos.y);
				Entity* current_entity = handle();
				
				// We don't want to double paint, so check to make sure we're not doing that
				bool okay_to_create = true;
				
				// If the tile we're painting over exists and is the same kind we're trying to paint, don't do it
				if (current_entity) {
					if (current_entity->name == selected()->name) {
						okay_to_create = false;
					}
				}
				
				if (okay_to_create) {
					// Create a lambda which will undo the tile placement we're about to do
					auto my_lambda =
						[&active_level = active_level,
						x = grid_pos.x, y = grid_pos.y,
						ent = active_level->get_tile(grid_pos.x, grid_pos.y)]
					{
						active_level->set_tile(ent, x, y);
					};
					action_stack.push_back(my_lambda);
					
					active_level->set_tile(selected, grid_pos.x, grid_pos.y);
					selected = Entity::create(selected()->name);
					translate();
					
					// Update so we only paint one entity per tile
					last_grid_drawn = grid_pos;
				}
			}
			else if (kind == ENTITY) {
				if (input.was_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
					auto my_lambda = [&active_level = active_level]() -> void {
						active_level->entities.pop_back();
					};
					action_stack.push_back(my_lambda);
					
					// Add the selection to the level
					active_level->entities.push_back(selected);
					
					// Create a new one of the same kind as the old one
					Entity* selection = selected();
					selected = Entity::create(selection->name);
					smooth_drag_offset = glm::vec2(0.f);
					
					// Translate it so it doesn't pop in at (0,0) for a frame
					translate();
				}
			}
		}
	}
	else if (state == EDIT) {
		draw_component_editor();
		if (input.was_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
			auto bounding_box = Center_Box::from_entity(selected);
			if (bounding_box) {
				// If you click inside the currently selected thing, start dragging it
				if (point_inside_box(input.world_pos, *bounding_box)) {
					smooth_drag_offset = get_cmp(selected, Position_Component)->world_pos - input.world_pos;
					state = DRAG;
				}
				// Otherwise, see if you clicked in something else.
				else {
					bool found = false;
					for (auto& handle : active_level->entities) {
						Entity* entity = handle();
						auto box = Center_Box::from_entity(handle);
						if (box) {
							if (point_inside_box(input.world_pos, *box)) {
								selected = handle;
								smooth_drag_offset = entity->get_component<Position_Component>()->world_pos - input.world_pos; // So we don't jump to the exact mouse position
								state = DRAG;
								found = true;
							}
						}
					}
					if (!found) {
						selected = { -1, nullptr };
						state = IDLE;
					}
				}
			}
		}
		// Way 1 to delete: Select, then right click
		else if (input.was_pressed(GLFW_MOUSE_BUTTON_RIGHT)) {
			delete_selected();
			state = IDLE;
		}
		
		// Way 2 to delete: Select, then 'delete'
		if (input.was_pressed(GLFW_KEY_DELETE)) {
			delete_selected();
			state = IDLE;
		}
	}
	else if (state == DRAG) {
		draw_component_editor();
		translate();
		
		if (!input.is_down[GLFW_MOUSE_BUTTON_LEFT]) {
			state = EDIT;
		}
	}
	else if (state == RECTANGLE_SELECT) {
		screen_unit top = top_left_drag.y > input.world_pos.y ? top_left_drag.y : input.world_pos.y;
		screen_unit bottom = top_left_drag.y > input.world_pos.y ? input.world_pos.y : top_left_drag.y;
		screen_unit right = top_left_drag.x > input.world_pos.x ? top_left_drag.x : input.world_pos.x;
		screen_unit left = top_left_drag.x > input.world_pos.x ? input.world_pos.x : top_left_drag.x;
		Points_Box points = { top, bottom, left, right };
		//@draw draw_square_outline(points, red);
		
		glm::vec2 dummy_penetration;
		Center_Box selection_area = Center_Box::from_points(points);
		for (auto& entity : active_level->entities) {
			auto entity_box = Center_Box::from_entity(entity);
			if (entity_box) {
				if (are_boxes_colliding(*entity_box, selection_area, dummy_penetration)) {
					draw_entity(entity, Render_Flags::Highlighted);
				}
			}
		}
	}
}
void Editor::render() {
	active_level->draw();
	if (selected) {
		draw_entity(selected, Render_Flags::Highlighted); 
	}
	// Actually make the draw calls to render all the tiles. Anything after this gets painted over it
	renderer.render_for_frame();
	
	// Render the grid
	if (show_grid) {
		screen_unit x_begin = fmodf(camera.offset.x, SCR_TILESIZE_X);
		screen_unit y_begin = fmodf(camera.offset.y, SCR_TILESIZE_Y) + (SCR_TILESIZE_Y / 2.f);
		
		for (float col_offset = 1 - x_begin; col_offset >= 0; col_offset -= SCR_TILESIZE_X) {
			draw_line_from_points(glm::vec2(col_offset, 0), glm::vec2(col_offset, 1), glm::vec4(.2f, .1f, .9f, 0.5f));
		}
		for (float row_offset = 1 - y_begin; row_offset >= 0; row_offset -= SCR_TILESIZE_Y) {
			draw_line_from_points(glm::vec2(0, row_offset), glm::vec2(1, row_offset), glm::vec4(.2f, .1f, .9f, 0.5f));
		}
		
	}
}
