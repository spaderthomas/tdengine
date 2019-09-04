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

	if (ImGui::CollapsingHeader("INFO")) {
		for (auto& [key, value] : layer_map) {
			if (value == active_layer)
				ImGui::Text("Layer: %s", key.c_str());
		}
	}
	
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

	// If this gets set, we know not to pipe the command down to the layer-specific handler
	bool ran_generic_command = false;

	auto copy = (char*)calloc(sizeof(command_line) + 1, sizeof(char));
	strcpy(copy, command_line);
	char* command = strtok(copy, " ");
	if (Stricmp(command, "layer") == 0) {
		ran_generic_command = true;
		char* which = strtok(NULL, " ");
		if (which) {
    		auto it = find_if(layer_map.begin(), layer_map.end(), [which](auto kvp) {
					return kvp.first == which;
				});

			if (it != layer_map.end()) {
				active_layer = layer_map[which];
				iactive_layer = std::distance(layer_map.begin(), it);
			}
		} else {
			AddLog("usage: layer {which}\n");
			AddLog("\toptions: editor, cutscene, game, battle\n");
		}
	} else if (Stricmp(command, "reload") == 0) {
		active_layer->reload();
	} else if (Stricmp(command, "screen") == 0) {
		char* res = strtok(NULL, " ");
		if (!res) {
			AddLog("format: screen {640, 720, 1080, 1440}");
			return;
		}
		
		if (!strcmp(res, "640")) use_640_360();
		else if (!strcmp(res, "720")) use_720p();
		else if (!strcmp(res, "1080")) use_1080p();
		else if (!strcmp(res, "1440")) use_1440p();
		else AddLog("format: screen {640, 720, 1080, 1440}");
	}
	
	if (!ran_generic_command) active_layer->exec_console_cmd(command_line);
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
	if (show_console) {
		console.Draw("tdconsole");
	}
	
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



void Cutscene::init(string name, TableNode* table) {
	this->name = name;
	this->level = levels[tds_string2(table, LEVEL_KEY)];
	this->level->load_entities(tds_table2(table, ENTITIES_KEY));

	TableNode* actions = tds_table2(table, ACTIONS_KEY);
	for (KVPNode* kvp : actions->assignments) {
		TableNode* action_table = (TableNode*)kvp->value;

		// Some actions have no attached entity.
		string entity_name = "";
		if (action_table->has_key(ENTITY_KEY)) {
			entity_name = tds_string2(action_table, ENTITY_KEY);
		}

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
	if (show_console) {
		console.Draw("tdconsole");
	}

	// @spader 8/22/2019: This is kind of hacky. I mean, we always want this to happen, but do I want
	// the camera to know what layer it's camera-ing for?
	camera.update(dt);
	input.world_pos = input.screen_pos + camera.offset;
	
	if (active_cutscene && !active_cutscene->done) {
		active_cutscene->update(dt);
	}
	
	physics_system.process(1.f / 60.f);
}

void Cutscene_Thing::render() {
	if (active_cutscene)
		active_cutscene->level->draw();
	
	renderer.render_for_frame();
	text_box.render();
}

void Cutscene_Thing::init() {
	for (auto& [name, cutscene] : cutscenes) {
		free(cutscene);
	}
	this->active_cutscene = nullptr;
	
	TableNode* all_cutscenes = tds_table(CUTSCENES_KEY);
	for (auto node : all_cutscenes->assignments) {
		KVPNode* kvp = (KVPNode*)node;
		Cutscene* cutscene = new Cutscene;
		cutscene->init(kvp->key, (TableNode*)kvp->value);
		this->cutscenes[kvp->key] = cutscene;
	}
}

void Cutscene_Thing::reload() {
	string old_active_cutscene = active_cutscene->name;
	ScriptManager.script_dir(absolute_path(path_join({"src", "scripts", "cutscenes"})));
	init();
	this->active_cutscene = cutscenes[old_active_cutscene];
}

void Cutscene_Thing::do_cutscene(string which) {
	auto cutscene = cutscenes[which];
	this->active_level = cutscene->level;
	this->active_cutscene = cutscene;
	this->camera.offset = { 0.f, 0.f };
}

void Cutscene_Thing::exec_console_cmd(char* command_line) {
	char* command = strtok(command_line, " ");
	if (!command) return;
	
	if (console.Stricmp(command, "cutscene") == 0) {
		char* which = strtok(NULL, " ");
		do_cutscene(which);
	} else if (console.Stricmp(command, "reload") == 0) {
		reload();
	} else {
		console.AddLog("Unknown command: '%s'. Loading up Celery Man instead.\n", command_line);
	}

}

// 
void Cutscene_Thing::exit() {
	this->active_level->load();
}
	
void Game::init() {
	particle_system.init();
	active_dialogue = new Dialogue_Tree;
	active_level = levels["overworld"]; //@hack Probably a better way to do this
	camera.following = g_hero;
}
void Game::update(float dt) {
	static int frame = 0;
	
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
