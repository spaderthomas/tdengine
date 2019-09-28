void Entity_Info::init() {
	auto entities_table = tds_table(ENTITY_KEY);
	for (auto node : entities_table->assignments) {
		KVPNode* kvp = (KVPNode*)node;
		this->entities.push_back(kvp->key);
		this->file_map[kvp->file].push_back(kvp->key);
		this->entity_to_file[kvp->key] = kvp->file;
		tdns_log.write(relative_path(kvp->file));
	}
}
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
	active_level = levels[tds_string(EDITOR_KEY, LEVEL_KEY)];
	camera.offset = glm::vec2 {
		tds_float(EDITOR_KEY, POS_KEY, "x"),
		tds_float(EDITOR_KEY, POS_KEY, "y")
	};
	tile_tree = Entity_Tree::create(absolute_path("textures/tiles"));
	entity_info.init();
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
				if (ImGui::ImageButton((ImTextureID)(uintptr_t)ent_sprite->atlas->handle,
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
	string copy = string(command_line);
	auto tokens = split(copy, ' ');
	if (tokens.empty()) {
		string message = "Ran into an error parsing command. ";
		message += "Command was: " + copy;
		console.AddLog(message.c_str());
	}

	if (tokens[0] == "save") {
		active_level->save();
	}
	else if (tokens[0] == "go") {
		string usage = "format: go [x: float] [y: float]";
		if (tokens.size() != 3) {
			console.AddLog(usage);
			return;
		}

		string x = tokens[1];
		if (x.empty()) {
			console.AddLog(usage);
			return;
		}
		
		string y = tokens[2];
		if (y.empty()) {
			console.AddLog(usage);
			return;
		}
		
		if (!is_float(x) || !is_float(y)) {
			console.AddLog(usage);
			return;
		}
		
		camera.offset = glm::vec2{stof(x), stof(y)};
	}
	else if (tokens[0] == "load") {
		active_level->load();
	}
	else if (tokens[0] == "grid") {
		if (kind == TILE && state != IDLE) {
			last_show_grid = !show_grid;
		} else {
			last_show_grid = show_grid;
		}
		show_grid = !show_grid;
	}
	else if (tokens[0] == "snap") {
		// If we're actually in tile mode, assume we want this new change to persist when we go back
		if (kind == TILE && state != IDLE) {
			last_snap_to_grid = !snap_to_grid;
		} else {
			last_snap_to_grid = snap_to_grid;
		}
		snap_to_grid = !snap_to_grid;
		
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
	reload_cutscenes();
	
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

	tds_set(camera.offset.x, EDITOR_KEY, POS_KEY, "x");
	tds_set(camera.offset.y, EDITOR_KEY, POS_KEY, "y");
	tds_set(active_level->name, EDITOR_KEY, LEVEL_KEY);
	
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
		ImGui::Begin("Entities", 0, flags);

		// File chooser + filter
		static ImGuiTextFilter file_filter;
		file_filter.Draw("Filter by Scripts");

		// Entity chooser + filter
		static ImGuiTextFilter entity_filter;
		entity_filter.Draw("Filter by Entity");
		
		// Populate a list of selectables that, when clicked, create the corresponding entity
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysAutoResize);

		vector<string> entities_to_display; // @copy
		for (auto& entity : entity_info.entities) {
			bool pass_entity_filter = entity_filter.PassFilter(entity.c_str());
			bool pass_file_filter = file_filter.PassFilter(entity_info.entity_to_file[entity].c_str());
			if (pass_entity_filter && pass_file_filter)
				entities_to_display.push_back(entity);
		}
		
		for (auto& entity : entities_to_display) {
			if (!entity_filter.PassFilter(entity.c_str())) continue;
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

		// Draw component editor for each entity
		if (ImGui::TreeNode("boon")) {
			g_hero->imgui_visualizer();
			ImGui::TreePop();
		}
		
		auto level = levels[level_current];
		for (auto entity : level->entities) {
			if (ImGui::TreeNode(entity->name.c_str())) {
				entity->imgui_visualizer();
				ImGui::TreePop();
			}
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
		ImGui::Begin("components", 0, ImGuiWindowFlags_AlwaysAutoResize);
		if (selected) selected->imgui_visualizer();
		ImGui::End();

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
		ImGui::Begin("components", 0, ImGuiWindowFlags_AlwaysAutoResize);
		if (selected) selected->imgui_visualizer();
		ImGui::End();
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

