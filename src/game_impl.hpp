
Entity_Tree* Entity_Tree::create(string dir) {
	Entity_Tree* tree = new Entity_Tree;
	tree->dir = name_from_full_path(dir);
	for (auto it = directory_iterator(dir); it != directory_iterator(); ++it) {
		string path = it->path().string();
		if (is_regular_file(it->status())) {
			if (is_png(path)) {
				string lua_id = strip_extension(name_from_full_path(path));
				tree->entities.push_back(Entity::create(lua_id));
				tree->size++;
			}
		}
		else if (is_directory(it->status())) {
			tree->children.push_back(create(path));
		}
	}

	return tree;
}
pool_handle<Entity> Entity_Tree::find(string lua_id) {
	for (auto& handle : entities) {
		if (handle()->lua_id == lua_id) {
			return handle;
		}
	}
	for (auto& child : children) {
		pool_handle<Entity> found_in_child = child->find(lua_id);
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
	filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
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
void  Console::ExecCommand(const char* command_line)
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

	// Process command
	if (Stricmp(command_line, "CLEAR") == 0)
	{
		ClearLog();
	}
	else if (Stricmp(command_line, "HELP") == 0)
	{
		AddLog("Commands:");
		for (int i = 0; i < Commands.Size; i++)
			AddLog("- %s", Commands[i]);
	}
	else if (Stricmp(command_line, "HISTORY") == 0)
	{
		int first = History.Size - 10;
		for (int i = first > 0 ? first : 0; i < History.Size; i++)
			AddLog("%3d: %s\n", i, History[i]);
	}
	else
	{
		game.exec_console_cmd(command_line);
	}
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

void Editor::init() {
	active_level = levels["overworld"];
	tile_tree = Entity_Tree::create(absolute_path("textures\\tiles"));
}
void Editor::translate() {
	get_cmp(selected, Position_Component)->world_pos =
		snap_to_grid ?
		screen_from_grid(grid_from_world(input.world_pos)) :
		input.world_pos + smooth_drag_offset;
}
void Editor::draw_component_editor() {
	// Leave the main tdengine window, and pop up a properties window for this entity
	ImGui::End();
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
						Level* level = kvp.second;
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
				ImGui::SliderFloat2("Position", glm::value_ptr(pc->world_pos), 0.f, 100.f);
				ImGui::TreePop();
			}
		}
	}

	// Re-enter main tdengine window
	ImGui::End();
	ImGui::Begin("tdengine", 0, ImGuiWindowFlags_AlwaysAutoResize);
}
void Editor::draw_tile_tree(Entity_Tree* root) {
	if (ImGui::CollapsingHeader("Tile Selector")) {
		for (auto child : root->children) {
			draw_tile_tree_recursive(child, 0);
		}
	}
}
int  Editor::draw_tile_tree_recursive(Entity_Tree* root, int unique_btn_index) { 
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
					selected = Entity::create(tile->lua_id);
					this->kind = TILE;
					this->state = INSERT;
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
void Editor::exec_console_cmd(const char* cmd) {
	if (console.Stricmp(cmd, "SAVE") == 0) {
		active_level->save();
	}
	else if (console.Stricmp(cmd, "LOAD") == 0) {
		active_level->load();
	}
	else if (console.Stricmp(cmd, "RELOAD") == 0) {
		reload_everything();
	}
	else {
		console.AddLog("Unknown command: '%s'. Loading up Celery Man instead.\n", cmd);
	}
}
void Editor::reload_lua() {
	Lua.init();
	active_level->load();
}
void Editor::reload_assets() {
	// @leak
	create_all_texture_atlas();
	tile_tree = Entity_Tree::create(absolute_path("textures\\tiles"));
	active_level->load();
}
void Editor::reload_everything() {
	reload_lua();
	reload_assets();
}
void Editor::undo() {
	if (stack.size()) {
		function<void()> undo = stack.back();
		stack.pop_back();
		undo();
	}
}
void Editor::update(float dt) {	
	if (input.was_pressed(GLFW_KEY_ESCAPE)) {
		selected = { -1, nullptr };
		state = IDLE;
	}
	if (input.is_down[GLFW_KEY_LEFT_ALT] &&
		input.was_pressed(GLFW_KEY_Z)) {
		undo();
	}

	#pragma region gui
	// Toggle the console on control
	if (global_input.was_pressed(GLFW_KEY_LEFT_CONTROL)) {
		show_console = !show_console;
	}
	if (show_console) {
		console.Draw("tdconsole");
	}

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
				if (ImGui::MenuItem("lua")) {
					reload_lua();
				}
				if (ImGui::MenuItem("assets")) {
					reload_assets();
				}
				if (ImGui::MenuItem("everything")) {
					reload_everything();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Windows")) {
				if (ImGui::MenuItem("FSM Debugger")) { 
					show_fsm_debugger = !show_fsm_debugger; 
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "M-z")) { undo(); }
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// Tile Selector GUI
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::Begin("tdengine", 0, flags);
	static glm::vec2 last_click = { 0, 0 };
	if (ImGui::CollapsingHeader("Options")) {
		ImGui::Checkbox("show grid", &show_grid);
		ImGui::Checkbox("snap to grid", &snap_to_grid);
		ImGui::Checkbox("show aabb", &debug_show_aabb);
		ImGui::Checkbox("show minkowski", &debug_show_minkowski);
		ImGui::Checkbox("show demo", &show_imgui_demo);
		ImGui::Text("Raw game input: %f, %f", input.screen_pos.x, input.screen_pos.y);
		ImGui::Text("Camera offset: %f, %f", camera.offset.x, camera.offset.y);
		ImGui::Text("Last click: %f, %f", last_click.x, last_click.y);
	}

	// Tile tree
	ImGui::Separator();
	draw_tile_tree(tile_tree);

	// Level selector
	ImGui::Separator();
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

	// Script selector
	ImGui::Separator();
	static string script_current = Lua.imgui_shown_scripts[0];
	if (ImGui::BeginCombo("##choosescript", (script_current + ".lua").c_str(), 0)) {
		for (auto script : Lua.imgui_shown_scripts) {
			bool is_selected = script == script_current;
			if (ImGui::Selectable((script + ".lua").c_str(), is_selected)) {
				script_current = script;
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysAutoResize);
	// Show each entity in this script. If we select it, create one and move to INSERT mode
	for (auto& ent : Lua.script_to_definitions[script_current]) {
		if (ImGui::Selectable(ent.c_str())) {
			selected = Entity::create(ent.c_str());
			this->kind = ENTITY;
			this->state = INSERT;
		}
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();

	// FSM debugger
	if (show_fsm_debugger) {
		if (selected) {
			Entity* entity = selected();
			State_Component* state = entity->get_component<State_Component>();

			// If the selection both exists and has a state component, we can run the FSM debugger
			if (state) {
				ImGui::Begin("FSM debugger", 0, ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::Text("Current State: "); ImGui::SameLine(); ImGui::Text(state->current_state.c_str());

				// Display each of the watched variables as a red or green button 
				for (auto& watched_variable : state->watched_variables) {
					bool current_status = knowledge_base[watched_variable];
					if (current_status) {
						static ImVec4 green_for_true = ImVec4(.06f, .8f, .05f, 1.f);
						ImGui::PushStyleColor(ImGuiCol_Button, green_for_true);
					}
					else {
						static ImVec4 red_for_false = ImVec4(1.f, .06f, .05f, 1.f);
						ImGui::PushStyleColor(ImGuiCol_Button, red_for_false);
					}

					// Toggle the button on click
					if (ImGui::Button(watched_variable.c_str())) {
						knowledge_base.update_variable(watched_variable, !current_status);
					}
					ImGui::PopStyleColor();
				}
				ImGui::End();
			}
		}
	}

	ImGui::End();
#pragma endregion

	#pragma region editing logic
	switch (state) {
	case IDLE: {
		if (input.was_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
			last_click = input.world_pos;
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
		break;
	}
	case INSERT: {
		translate();

		// And if we click, add it to the level
		if (input.is_down[GLFW_MOUSE_BUTTON_LEFT]) {
			switch (kind) {
			case TILE: {
				auto grid_pos = grid_from_world(input.world_pos);
				pool_handle<Entity> handle = active_level->get_tile(grid_pos.x, grid_pos.y);
				Entity* current_entity = handle();

				// We don't want to double paint, so check to make sure we're not doing that
				bool okay_to_create = true;

				// If the tile we're painting over exists and is the same kind we're trying to paint, don't do it
				if (current_entity) {
					if (current_entity->lua_id == selected()->lua_id) {
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
								stack.push_back(my_lambda);

								active_level->set_tile(selected, grid_pos.x, grid_pos.y);
								selected = Entity::create(selected()->lua_id);

								// Update so we only paint one entity per tile
								last_grid_drawn = grid_pos;
								break;
				}
				break;
			}
			case ENTITY: {
				if (input.was_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
					auto my_lambda = [&active_level = active_level]() -> void {
						active_level->entities.pop_back();
					};
					stack.push_back(my_lambda);

					// Add the selection to the level
					active_level->entities.push_back(selected);

					// Create a new one of the same kind as the old one
					Entity* selection = selected();
					selected = Entity::create(selection->lua_id);
					smooth_drag_offset = glm::vec2(0.f);
					translate();
				}
				break;
			}
			}
		}
		break;
	}
	case EDIT: {
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

		// Delete whatever is selected
		if (input.was_pressed(GLFW_KEY_DELETE)) {
			for (auto it = active_level->entities.begin(); it != active_level->entities.end(); it++) {
				if (selected == *it) {
					selected->clear_components();
					active_level->entities.erase(it);
					break;
				}
			}
			selected = { -1, nullptr };
			state = IDLE;
		}
		break;
	}
	case DRAG: {
		draw_component_editor();
		translate();

		if (!input.is_down[GLFW_MOUSE_BUTTON_LEFT]) {
			state = EDIT;
		}
		break;
	}
	case RECTANGLE_SELECT: {
		screen_unit top = top_left_drag.y > input.world_pos.y ? top_left_drag.y : input.world_pos.y;
		screen_unit bottom = top_left_drag.y > input.world_pos.y ? input.world_pos.y : top_left_drag.y;
		screen_unit right = top_left_drag.x > input.world_pos.x ? top_left_drag.x : input.world_pos.x;
		screen_unit left = top_left_drag.x > input.world_pos.x ? input.world_pos.x : top_left_drag.x;
		Points_Box points = { top, bottom, left, right };
		//@draw draw_square_outline(points, red);

		glm::vec2 dummy_penetration;
		Center_Box selection_area = Center_Box::from_points(points);
		for (auto& handle : active_level->entities) {
			auto entity_box = Center_Box::from_entity(handle);
			if (entity_box) {
				if (are_boxes_colliding(*entity_box, selection_area, dummy_penetration)) {
					handle()->draw(Render_Flags::Highlighted);
				}
			}
		}

		break;
	}
	}
	#pragma endregion
}
void Editor::render() {
	active_level->draw();
	if (selected) { 
		selected()->draw(Render_Flags::Highlighted); 
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


void Game::init() {
	particle_system.init();
	active_level = levels["overworld"];
	scene = "intro";
	active_dialogue = new Dialogue_Tree;
}
void Game::update(float dt) {
	static int frame = 0;

	for (auto& task : tasks) {
		task.update(dt);
	}

	// Toggle the console
	if (global_input.was_pressed(GLFW_KEY_LEFT_CONTROL)) {
		show_console = !show_console;
	}
	if (show_console) {
		console.Draw("tdnsconsole");
	}
	
	// Set up the camera so that the entity it's following is centered
	def_get_cmp(follow_pc, camera.following.deref(), Position_Component);
	camera.offset = follow_pc->world_pos;
	camera.offset += glm::vec2{-.5, -.5};
	input.world_pos = input.screen_pos + camera.offset;

	Lua.run_game_update(dt);
	physics_system.process(1.f / 60.f);
	
	if (false) {
		
	}

	text_box.update(frame);
	frame++;
}
void Game::render() {
	renderer.render_for_frame();
	text_box.render();
}