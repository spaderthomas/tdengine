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
Entity* Entity_Tree::find(string lua_id) {
	for (auto& entity : entities) {
		if (entity->lua_id == lua_id) {
			return entity;
		}
	}
	for (auto& child : children) {
		Entity* found_in_child = child->find(lua_id);
		if (found_in_child) {
			return found_in_child;
		}
	}

	return nullptr;
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
Console::~Console()
{
	ClearLog();
	for (int i = 0; i < History.Size; i++)
		free(History[i]);
}
int Console::Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
int Console::Strnicmp(const char* str1, const char* str2, int n) {
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
void Console::ClearLog()
{
	for (int i = 0; i < Items.Size; i++)
		free(Items[i]);
	Items.clear();
	ScrollToBottom = true;
}
void Console::AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
	// FIXME-OPT
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	Items.push_back(Strdup(buf));
	ScrollToBottom = true;
}
void Console::Draw(const char* title)
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
int Console::TextEditCallbackStub(ImGuiTextEditCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
{
	Console* console = (Console*)data->UserData;
	return console->TextEditCallback(data);
}
int Console::TextEditCallback(ImGuiTextEditCallbackData* data)
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
void Console::ExecCommand(const char* command_line)
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
		game_layer.exec_console_command(command_line);
	}
}


void Player::init() {
	boon = Entity::create("boon");
}
void Player::update(float dt) {
	frame_timer -= dt;
	if (frame_timer <= 0.f) {
		auto gc = boon->get_component<Graphic_Component>();
		auto animation = gc->active_animation;
		animation->next_frame();
		frame_timer = 8.f / 60.f;
	}
}

void Circle_Buffer::push_back(int elem) {
	fox_assert(len <= capacity);
	fox_assert(head >= 0 && head < capacity);
	if (len == capacity) { return; }
	data[(head + len) % capacity] = elem;
	len++;
}
optional<int> Circle_Buffer::pop_front() {
	fox_assert(len <= capacity);
	fox_assert(head >= 0 && head < capacity);
	if (len) {
		int ret = data[head];
		head = (head + 1) % capacity;
		len--;
		return ret;
	}
	return {};
}
void Circle_Buffer::clear() {
	memset(data, 0, sizeof(data));
	head = 0;
	len = 0;
}

float Sine_Func::eval_at(float point) {
	return amp * glm::sin(period * point - phase_shift);
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

void Game::Editor_Selection::translate_entity() {
	glm::vec2 draggable_position;
	glm::ivec2 grid_pos = grid_pos_from_px_pos(game_input.px_pos) + camera;
	if (snap_to_grid) {
		draggable_position = screen_from_grid(grid_pos);
	}
	else {
		draggable_position = game_input.screen_pos + smooth_drag_offset;
	}

	Position_Component* pc = selection->get_component<Position_Component>();
	pc->screen_pos = draggable_position;
}
void Game::Editor_Selection::draw_component_editor() {
	// Leave the main tdengine window, and pop up a properties window for this entity
	ImGui::End();
	ImGui::Begin("components", 0, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text(string("/scripts/" + Lua.definitions_to_script[selection->lua_id] + "/" + selection->lua_id).c_str());
	for (auto& component : selection->components) {
		if (dynamic_cast<Graphic_Component*>(component)) {
			if (ImGui::TreeNode("Graphic Component")) {
				Graphic_Component* gc = dynamic_cast<Graphic_Component*>(component);
				Animation* current_animation = gc->active_animation;
				if (ImGui::BeginCombo("Animations", current_animation->name.c_str(), 0)) {
					for (auto anim : gc->animations) {
						bool is_selected = anim->name == current_animation->name;
						if (ImGui::Selectable(anim->name.c_str(), is_selected)) {
							gc->set_animation(anim->name);
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::SliderFloat2("Scale", glm::value_ptr(gc->scale), 0.f, 1.f);
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
	ImGui::Begin("tdengine", 0, ImGuiWindowFlags_AlwaysAutoResize);
}
void Game::play_intro() {
	Center_Box full_screen;
	full_screen.origin = glm::vec2(.5f, .5f);
	full_screen.extents = glm::vec2(1.f);
	draw_square(full_screen, black);

	string text = Lua.state["intro"]["text"];
	text_box.begin(text);
	/*
	dimensions of the text area
	which text box to use
	*/
}
void Game::reload() {
	//@leak this one is quite big 
	for (auto dirname : atlas_folders) {
		create_texture_atlas(dirname);
	}

	Lua.init();

	tile_tree = Entity_Tree::create("..\\..\\textures\\tiles");

	// Reload all tiles
	for (auto it : active_level->chunks) {
		Chunk& chunk = it.second;
		fox_for(x, CHUNK_SIZE) {
			fox_for(y, CHUNK_SIZE) {
				Entity* entity = chunk.tiles[x][y];
				if (entity != nullptr) {
					for (auto& component : entity->components) {
						string script = Lua.definitions_to_script[entity->lua_id];
						component->init_from_table(Lua.state[script][entity->lua_id][component->name()]);
					}
				}
			}
		}
	}

	// Reload all entities
	for (auto& entity : active_level->entities) {
		for (auto& component : entity->components) {
			string script = Lua.definitions_to_script[entity->lua_id];
			component->init_from_table(Lua.state[script][entity->lua_id][component->name()]);
		}
	}
}
void Game::undo() {
	if (stack.size()) {
		function<void()> undo = stack.back();
		stack.pop_back();
		undo();
	}
}
void Game::exec_console_command(const char* command_line) {
	if (console.Stricmp(command_line, "SAVE") == 0) {
		active_level->save();
	}
	else if (console.Stricmp(command_line, "LOAD") == 0) {
		active_level->load();
	}
	else if (console.Stricmp(command_line, "RELOAD") == 0) {
		reload();
	}
	else {
		console.AddLog("Unknown command: '%s'. Loading up Celery Man instead.\n", command_line);
	}
}
void Game::init() {
	editor_state = IDLE;
	tile_tree = Entity_Tree::create("..\\..\\textures\\tiles");
	dude_ranch.name = "dude_ranch";
	cantina.name = "cantina";
	active_level = &cantina;
	player.init();
	particle_system.init();
	
	sol::table test = Lua.state["npc"]["wilson"]["State_Component"];
	State_Component sc;
	sc.init_from_table(test);
	sc.update(&sc, "fasf", true);
}
void Game::update(float dt) {
	static int frame = 0;
	active_level->draw();

	//--INPUT
	if (game_input.is_down[GLFW_KEY_UP]) {
		camera.y = fox_max(0, camera.y - 1);
	}
	if (game_input.is_down[GLFW_KEY_DOWN]) {
		camera.y = camera.y + 1;
	}
	if (game_input.is_down[GLFW_KEY_RIGHT]) {
		camera.x += 1;
	}
	if (game_input.is_down[GLFW_KEY_LEFT]) {
		camera.x = fox_max(0, camera.x - 1);
	}
	if (game_input.is_down[GLFW_KEY_LEFT_ALT] &&
		game_input.was_pressed(GLFW_KEY_Z))
	{
		undo();
	}
	if (game_input.was_pressed(GLFW_KEY_ESCAPE)) {
		editor_selection.selection = nullptr;
		editor_state = IDLE;
	}
	if (game_input.was_pressed(GLFW_MOUSE_BUTTON_RIGHT)) {
		particle_system.start();
	}

	// Toggle the console on control
	if (global_input.was_pressed(GLFW_KEY_LEFT_CONTROL)) {
		show_console = !show_console;
	}
	if (show_console) {
		console.Draw("tdnsconsole");
	}



	//--GUI (global stuff goes here -- obviously since ImGui smaller things can be put anywhere!)
	// Top menu
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "console::save")) { active_level->save(); }
			if (ImGui::MenuItem("Load", "console::load")) { active_level->load(); }
			if (ImGui::MenuItem("Reload", "console::reload")) { reload(); }
			if (ImGui::BeginMenu("Windows")) {
				if (ImGui::MenuItem("FSM Debugger")) { show_fsm_debugger = !show_fsm_debugger; }
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
	if (ImGui::CollapsingHeader("Options")) {
		ImGui::Checkbox("Show grid", &show_grid);
		ImGui::Checkbox("Snap to grid", &snap_to_grid);
		ImGui::Checkbox("Show AABBs", &debug_show_aabb);
		ImGui::Checkbox("Show Minkowski", &debug_show_minkowski);
		ImGui::Checkbox("Show ImGui demo", &show_imgui_demo);
	}

	// Tile tree
	ImGui::Separator();
	int unique_button_index = 0; // Needed for ImGui
	auto draw_tile_tree = [&](Entity_Tree* root) -> void {
		auto lambda = [&](Entity_Tree* root, const auto& lambda) -> void {
			if (ImGui::TreeNode(root->dir.c_str())) {

				// If expanded, draw this folder's tiles
				int indx = 0;
				for (auto tile : root->entities) {
					Graphic_Component* gc = tile->get_component<Graphic_Component>();
					if (gc) {
						Sprite* ent_sprite = gc->get_current_frame();

						ImVec2 top_right_tex_coords = ImVec2(ent_sprite->tex_coords[2], ent_sprite->tex_coords[3]);
						ImVec2 bottom_left_tex_coords = ImVec2(ent_sprite->tex_coords[6], ent_sprite->tex_coords[7]);
						ImVec2 button_size = ImVec2(32, 32);
						ImGui::PushID(unique_button_index++);
						if (ImGui::ImageButton((ImTextureID)ent_sprite->atlas->handle,
							button_size,
							bottom_left_tex_coords, top_right_tex_coords))
						{
							editor_selection.selection = Entity::create(tile->lua_id);
							editor_selection.kind = Editor_Selection::TILE;
							editor_state = INSERT;
						}
						ImGui::PopID();
						bool is_end_of_row = !((indx + 1) % 6);
						bool is_last_element = indx == (root->size - 1);
						if (!is_end_of_row && !is_last_element) { ImGui::SameLine(); }
						indx++;
					}
				}

				// And draw dropdowns for any children
				for (auto child : root->children) {
					lambda(child, lambda);
				}

				ImGui::TreePop();
			}
		};
		if (ImGui::CollapsingHeader("Tile Selector")) {
			for (auto child : root->children) {
				lambda(child, lambda);
			}
		}
	};
	draw_tile_tree(tile_tree);

	// Level selector
	ImGui::Separator();
	static string level_current = "choose level";
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
	static string script_current = Lua.scripts[0];
	if (ImGui::BeginCombo("##choosescript", (script_current + ".lua").c_str(), 0)) {
		for (auto script : Lua.scripts) {
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
			editor_selection.selection = Entity::create(ent.c_str());
			editor_selection.kind = Editor_Selection::ENTITY;
			editor_state = INSERT;
		}
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();

	// FSM debugger
	if (show_fsm_debugger) {
#if 0
		if (editor_selection.selection) {
			auto fsm = editor_selection.selection
		}
		ImGui::Begin("FSM debugger", 0, ImGuiWindowFlags_AlwaysAutoResize);
		sol::table dialogue_fsm = Lua.state["wilson"]["State_Component"];
		string current_state = dialogue_fsm["current_state"];
		ImGui::Text("Current State: "); ImGui::SameLine(); ImGui::Text(current_state.c_str());
		sol::table watched = dialogue_fsm.get<sol::table>("watched_variables");
		for (auto& watched_entry : watched) {
			string watched_variable = watched_entry.second.as<string>();
			bool current_status = knowledge_base[watched_variable];
			if (current_status) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(.06f, .8f, .05f, 1.f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, .06f, .05f, 1.f));
			}
			if (ImGui::Button(watched_variable.c_str())) {
				knowledge_base.update_variable(watched_variable, !current_status);
			}
			ImGui::PopStyleColor();
		}
		ImGui::End();
#endif
	}

	//--EDITOR
	switch (editor_state) {
	case Editing_State::IDLE: {
		if (game_input.was_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
			bool clicked_inside_something = false;
			for (auto& entity : active_level->entities) {
				auto box = Center_Box::from_entity(entity);
				if (box) {
					if (point_inside_box(game_input.screen_pos, *box)) {
						clicked_inside_something = true;
						editor_selection.selection = entity;
						editor_selection.smooth_drag_offset = entity->get_component<Position_Component>()->screen_pos - game_input.screen_pos; // So we don't jump to the exact mouse position
						editor_state = DRAG;
						break;
					}
				}
			}

			if (!clicked_inside_something) {
				top_left_drag = game_input.screen_pos;
				editor_state = RECTANGLE_SELECT;
			}
		}
		break;
	}
	case Editing_State::INSERT: {
		editor_selection.translate_entity();

		// And if we click, add it to the level
		if (game_input.is_down[GLFW_MOUSE_BUTTON_LEFT]) {
			switch (editor_selection.kind) {
			case Editor_Selection::TILE: {
				auto grid_pos = grid_pos_from_px_pos(game_input.px_pos);
				Entity* current_entity = active_level->get_tile(grid_pos.x, grid_pos.y);

				// We don't want to double paint, so check to make sure we're not doing that
				bool okay_to_create = true;

				// If the tile we're painting over exists and is the same kind we're trying to paint, don't do it
				if (current_entity) {
					if (current_entity->lua_id == editor_selection.selection->lua_id) {
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

								active_level->set_tile(editor_selection.selection, grid_pos.x, grid_pos.y);
								editor_selection.selection = Entity::create(editor_selection.selection->lua_id);

								// Update so we only paint one entity per tile
								last_grid_pos_drawn = grid_pos;
								break;
				}
				break;
			}
			case Editor_Selection::ENTITY: {
				if (game_input.was_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
					auto my_lambda = [&active_level = active_level]() -> void {
						active_level->entities.pop_back();
					};
					stack.push_back(my_lambda);

					active_level->entities.push_back(editor_selection.selection);
					editor_selection.selection = Entity::create(editor_selection.selection->lua_id);
					editor_selection.smooth_drag_offset = glm::vec2(0.f);
					editor_selection.translate_entity();
				}
				break;
			}
			}
		}
		break;
	}
	case Editing_State::EDIT: {
		editor_selection.draw_component_editor();
		if (game_input.was_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
			auto bounding_box = Center_Box::from_entity(editor_selection.selection);
			if (bounding_box) {
				// If you click inside the currently selected thing, start dragging it
				if (point_inside_box(game_input.screen_pos, *bounding_box)) {
					editor_selection.smooth_drag_offset = editor_selection.selection->get_component<Position_Component>()->screen_pos - game_input.screen_pos;
					editor_state = DRAG;
				}
				// Otherwise, see if you clicked in something else.
				else {
					bool found = false;
					for (auto& entity : active_level->entities) {
						auto box = Center_Box::from_entity(entity);
						if (box) {
							if (point_inside_box(game_input.screen_pos, *box)) {
								editor_selection.selection = entity;
								editor_selection.smooth_drag_offset = entity->get_component<Position_Component>()->screen_pos - game_input.screen_pos; // So we don't jump to the exact mouse position
								editor_state = DRAG;
								found = true;
							}
						}
					}
					if (!found) {
						editor_selection.selection = nullptr;
						editor_state = IDLE;
					}
				}
			}
		}

		// Delete whatever is selected
		if (game_input.was_pressed(GLFW_KEY_DELETE)) {
			for (auto it = active_level->entities.begin(); it != active_level->entities.end(); it++) {
				if (editor_selection.selection == *it) {
					active_level->entities.erase(it);
					break;
				}
			}
			editor_selection.selection = nullptr;
			editor_state = IDLE;
		}
		break;
	}
	case Editing_State::DRAG: {
		editor_selection.draw_component_editor();
		editor_selection.translate_entity();

		if (!game_input.is_down[GLFW_MOUSE_BUTTON_LEFT]) {
			editor_state = EDIT;
		}
		break;
	}
	case Editing_State::RECTANGLE_SELECT: {
		screen_unit top = top_left_drag.y > game_input.screen_pos.y ? top_left_drag.y : game_input.screen_pos.y;
		screen_unit bottom = top_left_drag.y > game_input.screen_pos.y ? game_input.screen_pos.y : top_left_drag.y;
		screen_unit right = top_left_drag.x > game_input.screen_pos.x ? top_left_drag.x : game_input.screen_pos.x;
		screen_unit left = top_left_drag.x > game_input.screen_pos.x ? game_input.screen_pos.x : top_left_drag.x;
		Points_Box points = { top, bottom, left, right };
		draw_square_outline(points, red);

		glm::vec2 dummy_penetration;
		Center_Box selection_area = Center_Box::from_points(points);
		for (auto& entity : active_level->entities) {
			auto entity_box = Center_Box::from_entity(entity);
			if (entity_box) {
				if (are_boxes_colliding(*entity_box, selection_area, dummy_penetration)) {
					entity->draw(Render_Flags::Highlighted);
				}
			}
		}

		break;
	}
	}
	ImGui::End();


	//--EXPLORATION
	auto mc = player.boon->get_component<Movement_Component>();
	bool moving = false;
	if (game_input.is_down[GLFW_KEY_W]) {
		mc->wish += glm::vec2(0.f, .0025f);
		auto gc = player.boon->get_component<Graphic_Component>();
		gc->set_animation2("walk_up");
		moving = true;
		player.facing = Facing::up;
	}
	if (game_input.is_down[GLFW_KEY_A]) {
		mc->wish += glm::vec2(-.0025f, 0.f);
		player.facing = Facing::left;
	}
	if (game_input.is_down[GLFW_KEY_S]) {
		mc->wish += glm::vec2(0.f, -.0025f);
		auto gc = player.boon->get_component<Graphic_Component>();
		gc->set_animation2("walk_down");
		moving = true;
		player.facing = Facing::down;
	}
	if (game_input.is_down[GLFW_KEY_D]) {
		mc->wish += glm::vec2(.0025f, 0.f);
		player.facing = Facing::right;
	}
	if (game_input.was_pressed(GLFW_KEY_SPACE)) {
		text_box.unwait();
	}
	if (game_input.was_pressed(GLFW_KEY_I)) {
		play_intro();
	}
	if (!moving) {
		auto gc = player.boon->get_component<Graphic_Component>();
		gc->set_animation2("stand");
	}

	Vision* vision_box = player.boon->get_component<Vision>();
	Points_Box points;
	auto pc = player.boon->get_component<Position_Component>();
	switch (player.facing) {
	case up:
		points.left = pc->screen_pos.x - vision_box->width;
		points.right = pc->screen_pos.x + vision_box->width;
		points.top = pc->screen_pos.y + 2 * vision_box->depth;
		points.bottom = pc->screen_pos.y;
		break;
	case down:
		points.left = pc->screen_pos.x - vision_box->width;
		points.right = pc->screen_pos.x + vision_box->width;
		points.top = pc->screen_pos.y;
		points.bottom = pc->screen_pos.y - 2 * vision_box->depth;
		break;
	default:
		points.left = pc->screen_pos.x - vision_box->width;
		points.right = pc->screen_pos.x + vision_box->width;
		points.top = pc->screen_pos.y + 2 * vision_box->depth;
		points.bottom = pc->screen_pos.y;
		break;
	}
	if (debug_show_aabb) {
		draw_square_outline(points, red);
	}

	if (game_input.was_pressed(GLFW_KEY_E)) {
		for (auto& ent : active_level->entities) {
			Center_Box boon = Center_Box::from_points(points);
			auto other = Center_Box::from_entity(ent);
			if (!other) { continue; }
			glm::vec2 sep;
			if (are_boxes_colliding(boon, *other, sep)) {
				auto ic = ent->get_component<Interaction_Component>();
				if (ic) {
					string script = Lua.definitions_to_script[ent->lua_id];
					sol::table ic_lua = Lua.state[script][ent->lua_id]["Interaction_Component"];
					ic->on_interact(ic_lua);
				}
			}
		}
	}

	renderer.draw(player.boon->get_component<Graphic_Component>(), player.boon->get_component<Position_Component>(), Render_Flags::None);
	for (auto& ent : active_level->entities) {
		if (ent->get_component<Bounding_Box>()) {
			physics_system.entities.push_back(ent);
		}

	}
	physics_system.entities.push_back(player.boon);

	physics_system.process(1.f / 60.f);

	particle_system.update(dt);

	player.update(dt);
	text_box.update(frame);
	frame++;


}
void Game::render() {
	if (editor_selection.selection) { editor_selection.selection->draw(Render_Flags::Highlighted); }

	// Actually make the draw calls to render all the tiles. Anything after this gets painted over it
	renderer.render_for_frame();

	// Render the grid
	if (show_grid) {
		// We have to multiply by two because OpenGL uses -1 to 1
		for (float col_offset = 0; col_offset <= 1; col_offset += SCR_TILESIZE_X) {
			draw_line_from_points(glm::vec2(col_offset, 0.f), glm::vec2(col_offset, 1.f), glm::vec4(.2f, .1f, .9f, 0.5f));
		}
		for (float row_offset = 0; row_offset <= 1; row_offset += SCR_TILESIZE_Y) {
			draw_line_from_points(glm::vec2(0.f, row_offset), glm::vec2(1.f, row_offset), glm::vec4(.2f, .1f, .9f, 0.5f));
		}
	}

	text_box.render();
}

