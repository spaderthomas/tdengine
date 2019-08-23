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

Console::Console()
{
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;
	Commands.push_back("HELP");
	Commands.push_back("HISTORY");
	Commands.push_back("CLEAR");
	AddLog("Welcome to Dear ImGui!");
}
Console::~Console() {}
int   Console::Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
int   Console::Strnicmp(const char* str1, const char* str2, int n) {
	int d = 0;
	while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) {
		str1++;
		str2++;
		n--;
	}
	return d;
}
char* Console::Strdup(const char *str) { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }
void  Console::Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }
void  Console::ClearLog()
{
	for (int i = 0; i < Items.Size; i++)
		free(Items[i]);
	Items.clear();
	ScrollToBottom = true;
}
void  Console::AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	Items.push_back(Strdup(buf));
	ScrollToBottom = true;
}
void  Console::Draw(const char* title)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title, NULL))
	{
		ImGui::End();
		return;
	}
	
	ImGui::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");
	
	if (ImGui::SmallButton("Clear")) { ClearLog(); } ImGui::SameLine();
	bool copy_to_clipboard = ImGui::SmallButton("Copy"); ImGui::SameLine();
	if (ImGui::SmallButton("Scroll to bottom")) ScrollToBottom = true;
	
	ImGui::Separator();
	
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	static ImGuiTextFilter filter;
	filter.Draw("Filter", 180);
	ImGui::PopStyleVar();
	ImGui::Separator();
	
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Clear")) ClearLog();
		ImGui::EndPopup();
	}
	
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	if (copy_to_clipboard)
		ImGui::LogToClipboard();
	
	// Apply special colors
	ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
	for (int i = 0; i < Items.Size; i++)
	{
		const char* item = Items[i];
		if (!filter.PassFilter(item))
			continue;
		ImVec4 col = col_default_text;
		if (strstr(item, "[error]")) col = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
		else if (strncmp(item, "# ", 2) == 0) col = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, col);
		ImGui::TextUnformatted(item);
		ImGui::PopStyleColor();
	}
	if (copy_to_clipboard)
		ImGui::LogFinish();
	if (ScrollToBottom)
		ImGui::SetScrollHere(1.0f);
	ScrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();
	
	// Command-line
	bool reclaim_focus = false;
	ImGui::SetKeyboardFocusHere(0);
	if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
	{
		Strtrim(InputBuf);
		if (InputBuf[0])
			ExecCommand(InputBuf);
		strcpy(InputBuf, "");
		reclaim_focus = true;
	}
	
	// Demonstrate keeping focus on the input box
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	
	ImGui::End();
}
int   Console::TextEditCallbackStub(ImGuiTextEditCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
{
	Console* console = (Console*)data->UserData;
	return console->TextEditCallback(data);
}
int   Console::TextEditCallback(ImGuiTextEditCallbackData* data)
{
	//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
	switch (data->EventFlag)
	{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			// Example of TEXT COMPLETION
			
			// Locate beginning of current word
			const char* word_end = data->Buf + data->CursorPos;
			const char* word_start = word_end;
			while (word_start > data->Buf)
			{
				const char c = word_start[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';')
					break;
				word_start--;
			}
			
			// Build a list of candidates
			ImVector<const char*> candidates;
			for (int i = 0; i < Commands.Size; i++)
				if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
				candidates.push_back(Commands[i]);
			
			if (candidates.Size == 0)
			{
				// No match
				AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
			}
			else if (candidates.Size == 1)
			{
				// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0]);
				data->InsertChars(data->CursorPos, " ");
			}
			else
			{
				// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
				int match_len = (int)(word_end - word_start);
				for (;;)
				{
					int c = 0;
					bool all_candidates_matches = true;
					for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
						if (i == 0)
						c = toupper(candidates[i][match_len]);
					else if (c == 0 || c != toupper(candidates[i][match_len]))
						all_candidates_matches = false;
					if (!all_candidates_matches)
						break;
					match_len++;
				}
				
				if (match_len > 0)
				{
					data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
				}
				
				// List matches
				AddLog("Possible matches:\n");
				for (int i = 0; i < candidates.Size; i++)
					AddLog("- %s\n", candidates[i]);
			}
			
			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
			const int prev_history_pos = HistoryPos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (HistoryPos == -1)
					HistoryPos = History.Size - 1;
				else if (HistoryPos > 0)
					HistoryPos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (HistoryPos != -1)
					if (++HistoryPos >= History.Size)
					HistoryPos = -1;
			}
			
			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != HistoryPos)
			{
				data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
				data->BufDirty = true;
			}
		}
	}
	return 0;
}
void  Console::ExecCommand(char* command_line)
{
	AddLog("# %s\n", command_line);
	
	// Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
	HistoryPos = -1;
	for (int i = History.Size - 1; i >= 0; i--)
		if (Stricmp(History[i], command_line) == 0)
	{
		free(History[i]);
		History.erase(History.begin() + i);
		break;
	}
	History.push_back(Strdup(command_line));
	
	active_layer->exec_console_cmd(command_line);
}


void Particle::make_alive() {
	velocity = glm::vec2(0.f);
	color = brown;
	y_baseline = rand_float(1.f);
	position = glm::vec2(0.f, y_baseline);
	y_offset.amp = rand_float(.5f);
	life = 1.f;
	alive = true;
}
void Particle_System::init() {
	free_list.data = (int*)malloc(1024 * sizeof(int));
	free_list.capacity = 1024;
}
void Particle_System::start() {
	fox_for(i, 1024) {
		free_list.push_back(i);
		particles[i].alive = false;
	}
}
void Particle_System::update(float dt) {
	// Emit 2 particles per frame
	fox_for(ipart, 2) {
		auto index = free_list.pop_front();
		if (index) {
			particles[*index].make_alive();
		}
	}
	
	fox_iter(it, particles) {
		auto& particle = *it;
		if (particle.alive) {
			Center_Box box = { particle.position, particle.scale };
			glm::vec4 energycolor = glm::vec4(particle.color.x, particle.color.y, particle.color.z, particle.life);
			draw_square(box, energycolor);
			
			particle.life -= dt;
			particle.position.x += dt;
			particle.position.y = particle.y_baseline + particle.y_offset.eval_at(particle.life);
			
			if (particle.life <= 0) {
				particle.alive = false;
				int index = it - particles.begin();
				free_list.push_back(index);
			}
		}
	}
}

void Battle::init() {
	battlers[0] = Entity::create("boi");
	battlers[1] = Entity::create("boi");
	
	fox_for (ibattler, 2) {
		auto pc = battlers[0]->get_component<Position_Component>();
		pc->world_pos = glm::vec2((ibattler ? 1.0f : -1.0f) * 0.25f, 0.0f);
	}
}

void Battle::render() {
	fox_for (ibattler, 2) {
		draw_entity(battlers[ibattler], Render_Flags::None);
	}
	
	renderer.render_for_frame();
}

void Battle::update(float dt) {
	camera.following = battlers[0];
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

	ImGui::Begin("Battle View!", 0, ImGuiWindowFlags_AlwaysAutoResize);
	int unique_btn_index = 0;
	fox_for(idx, 2) {
		EntityHandle actor = battlers[idx];
		auto bc = actor->get_component<BattleComponent>();
		ImGui::Text("Player %d", idx);
		ImGui::Text("Health: %d", bc->health);
		
		ImGui::Text("Moves:");
		for (auto move : bc->moves) {
			string unique_move_id = move->name + to_string(unique_btn_index);
			unique_btn_index++;
			if (ImGui::Button(unique_move_id.c_str())) {
				EntityHandle other = idx ? battlers[0] : battlers[1];
				auto other_bc = other->get_component<BattleComponent>();
				other_bc->health -= move->power;
			}
		}
	}
	ImGui::End();
	
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
				if (ImGui::BeginCombo("##setdoor", door->to.c_str(), 0)) {
					for (auto& kvp : levels) {
						const string& name = kvp.first;
						bool is_selected = door->to.c_str() == name;
						if (ImGui::Selectable(name.c_str(), &is_selected)) {
							door->to = name;
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
	else if (console.Stricmp(command, "screen") == 0) {
		char* res = strtok(NULL, " ");
		if (!res) {
			console.AddLog("format: screen {640, 720, 1080, 1440}");
			return;
		}
		
		if (!strcmp(res, "640")) use_640_360();
		else if (!strcmp(res, "720")) use_720p();
		else if (!strcmp(res, "1080")) use_1080p();
		else if (!strcmp(res, "1440")) use_1440p();
		else console.AddLog("format: screen {640, 720, 1080, 1440}");
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
	else if (console.Stricmp(command, "reload") == 0) {
		reload_everything();
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
		string level_name = strtok(NULL, " ");
		
		bool is_valid_level_name = false;
		for (auto& kvp : levels) {
			if (kvp.first == level_name) {
				is_valid_level_name = true;
				break;
			}
		}
		if (!is_valid_level_name) {
			console.AddLog("Invalid level name");
			return;
		}
		
		this->active_level = levels[level_name];
		active_layer->active_level = levels[level_name];
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
void Editor::reload_everything() {
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
	
#if 0
	// Set up the camera so that the entity it's following is centered
	def_get_cmp(follow_pc, camera.following.deref(), Position_Component);
	camera.offset = follow_pc->world_pos;
	camera.offset += glm::vec2{ -.5, -.5 };
	input.world_pos = input.screen_pos + camera.offset;
#endif
	
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
	if (global_input.was_pressed(GLFW_KEY_LEFT_CONTROL)) {
		show_console = !show_console;
	}
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
					reload_everything();
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

void Cutscene::init(TableNode* table) {
	this->level = levels[tds_string2(table, LEVEL_KEY)];
	this->level->load_entities(tds_table2(table, ENTITIES_KEY));

	TableNode* actions = tds_table2(table, ACTIONS_KEY);
	for (KVPNode* kvp : actions->assignments) {
		TableNode* action_table = (TableNode*)kvp->value;
		string entity_name = tds_string2(action_table, ENTITY_KEY);
		
		EntityHandle entity = this->level->get_first_matching_entity(entity_name);
		Action* action = action_from_table(action_table, entity);
		this->task.add_action(action);
	}
}

void Cutscene::update(float dt) {
	frame++;
	task.update(dt);
}

void Cutscene_Thing::update(float dt) {
	// @spader 8/22/2019: This is kind of hacky. I mean, we always want this to happen, but do I want
	// the camera to know what layer it's camera-ing for?
	camera.update(dt);
	input.world_pos = input.screen_pos + camera.offset;
	
	if (!active_cutscene->done) {
		active_cutscene->update(dt);
	}
	
	physics_system.process(1.f / 60.f);
}

void Cutscene_Thing::render() {
	if (active_cutscene)
		active_cutscene->level->draw();
	
	renderer.render_for_frame();
}

void Cutscene_Thing::init() {
	TableNode* cutscene_data = tds_table("cutscenes", "test");
	Cutscene* cutscene = new Cutscene; // @leak if we call this 2ce
	cutscene->init(cutscene_data);
	this->active_level = levels[tds_string2(cutscene_data, LEVEL_KEY)];
	this->cutscenes["test"] = cutscene;
	this->active_cutscene = cutscene;
	this->camera.offset = { 0.f, 0.f };
}

void Cutscene_Thing::reload() {
	ScriptManager.script_dir(absolute_path(path_join({"src", "scripts", "cutscenes"})));
	init();
}

void Game::init() {
	particle_system.init();
	active_dialogue = new Dialogue_Tree;
	active_level = levels["overworld"]; //@hack Probably a better way to do this
	camera.following = g_hero;
}
void Game::update(float dt) {
	static int frame = 0;
	
	// Toggle the console
	if (global_input.was_pressed(GLFW_KEY_LEFT_CONTROL)) {
		show_console = !show_console;
	}
	if (show_console) {
		console.Draw("tdconsole");
	}
	
	// Set up the camera so that the entity it's following is centered
	camera.update(dt);
	input.world_pos = input.screen_pos + camera.offset;

	for (auto entity : active_level->entities) {
		update_task(entity, dt);
	}
	
	if (in_dialogue) {
	} else {
		// Deal with the player
		move_entity(g_hero, input.is_down[GLFW_KEY_W], input.is_down[GLFW_KEY_S], input.is_down[GLFW_KEY_A], input.is_down[GLFW_KEY_D]);
		draw_entity(g_hero, Render_Flags::None);
		
		// Handle collisions
		for (auto& entity : active_level->entities) {
			if (entity->get_component<Collision_Component>()) {
				Collision_Element new_collision_element;
				new_collision_element.me = g_hero;
				new_collision_element.other = entity;
				physics_system.collisions.push_back(new_collision_element);
			}
		}
		
		physics_system.process(1.f / 60.f);
	}
	
	frame++;
}
void Game::render() {
	active_level->draw();
	renderer.render_for_frame();
	text_box.render();
}
