struct Console {
    char                  InputBuf[256];
    ImVector<char*>       Items;
    bool                  ScrollToBottom;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char*> Commands;

    Console()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        AddLog("Welcome to Dear ImGui!");
    }
    ~Console()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            free(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* str1, const char* str2)         { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
    static int Strnicmp(const char* str1, const char* str2, int n) { 
		int d = 0; 
		while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) {
			 str1++; 
			 str2++; 
			 n--; 
		} 
		return d; 
	}
    static char* Strdup(const char *str)                             { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }
    static void  Strtrim(char* str)                                  { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

	void ClearLog()
	{
		for (int i = 0; i < Items.Size; i++)
			free(Items[i]);
		Items.clear();
		ScrollToBottom = true;
	}

	void AddLog(const char* fmt, ...) IM_FMTARGS(2)
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

	void Draw(const char* title, bool* p_open)
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
		// Here we create a context menu only available from the title bar.
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Close"))
				*p_open = false;
			ImGui::EndPopup();
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
	void ExecCommand(const char* command_line);

	static int TextEditCallbackStub(ImGuiTextEditCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
	{
		Console* console = (Console*)data->UserData;
		return console->TextEditCallback(data);
	}
	int TextEditCallback(ImGuiTextEditCallbackData* data)
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
    
};

vector<function<void()>> stack;
struct {
	vector<Tilemap*> layers;
	int indx_active_layer = -1;
	glm::ivec2 last_grid_pos_drawn;
	enum {
		IDLE,
		INSERT,
		SELECT
	} editing_state;
	Entity* draggable_entity = nullptr;
	string id_selected_entity;

	Console console;

	void exec_console_command(const char* command_line) {
		// addlayer layername
		// Creates a new tilemap named layername and adds it to the current level
		if (console.Strnicmp(command_line, "ADDLAYER", 8) == 0) {
			Tilemap* new_layer = new Tilemap;
			
			// Pars
			const char* c = &command_line[9]; 
			while (*c) {
				new_layer->name += *c;
				c++;
			}

			layers.push_back(new_layer);
		}
		else if (console.Stricmp(command_line, "SAVE") == 0) {
			if (indx_active_layer > -1) {
				layers[indx_active_layer]->save();
			}
		}
		else if (console.Stricmp(command_line, "LOAD") == 0) {
			if (indx_active_layer > -1) {
				layers[indx_active_layer]->load();
			}
		}
		else if (console.Stricmp(command_line, "RELOAD") == 0) {
			init_template_entities();

			//@leak this one is quite big 
			create_texture_atlas("../../textures/environment");
			create_texture_atlas("../../textures/boon");
			create_texture_atlas("../../textures/wilson");

			Lua.load_scripts();
			// Reload all graphics components
			fox_for(x, MAP_SIZE) {
				fox_for(y, MAP_SIZE) {
					Entity* cur = layers[indx_active_layer]->tiles[x][y];
					if (cur != nullptr) {
						Graphic_Component* gc = cur->get_component<Graphic_Component>();
						if (gc) {
							gc->load_animations_from_lua(Lua.state[cur->lua_id]["Graphic_Component"]);
						}
					}
				}
			}
		}
		else if (console.Stricmp(command_line, "CLEAN") == 0) {
			fox_for(x, MAP_SIZE) {
				fox_for(y, MAP_SIZE) {
					layers[indx_active_layer]->tiles[x][y] = nullptr;
				}
			}
		}
		else {
			console.AddLog("Unknown command: '%s'. Loading up Celery Man instead.\n", command_line);
		}
	}

	void init() {
		editing_state = IDLE;
		layers.push_back(new Tilemap);
		layers.push_back(new Tilemap);
		layers.push_back(new Tilemap);
		layers[0]->name = "layer_one";
		layers[1]->name = "layer_two";
		layers[2]->name = "pathtest";
		indx_active_layer = 0;
	}
	
	void update(float dt) {
		if (game_input.was_pressed(GLFW_KEY_UP)) {
				camera_pos.y -= GLSCR_TILESIZE_Y;
			}
		if (game_input.was_pressed(GLFW_KEY_DOWN)) {
				camera_pos.y += GLSCR_TILESIZE_Y;
			}
		if (game_input.was_pressed(GLFW_KEY_RIGHT)) {
				camera_pos.x -= GLSCR_TILESIZE_X;
			}
		if (game_input.was_pressed(GLFW_KEY_LEFT)) {
			camera_pos.x += GLSCR_TILESIZE_X;
		}
		if (game_input.was_pressed(GLFW_KEY_TAB)) {
			indx_active_layer = (indx_active_layer + 1) % layers.size();
		}
		if (game_input.is_down[GLFW_KEY_LEFT_ALT] &&
			game_input.was_pressed(GLFW_KEY_Z)) 
		{
			if (stack.size()) {
				function<void()> undo = stack.back();
				stack.pop_back();
				undo();
			}
		}
		if (game_input.was_pressed(GLFW_KEY_ESCAPE)) {
			if (editing_state == SELECT) {
				editing_state = INSERT;
			}
			else {
				editing_state = SELECT;
			}
		}
		
		// Toggle the console on control
		static bool show_console = true;
		static bool console_close = false;
		if (global_input.was_pressed(GLFW_KEY_LEFT_CONTROL)) {
			show_console = !show_console;
		}
		if (show_console) {
			console.Draw("tdnsconsole", &console_close);
		}

		// Tile Selector GUI
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::Begin("Tile Editor", 0, flags);
		fox_for(indx, template_tiles.size()) {
			Entity* template_tile = template_tiles[indx];
			Graphic_Component* gc = template_tile->get_component<Graphic_Component>();
			if (gc) {
				Sprite* ent_sprite = gc->get_current_frame();

 				ImVec2 top_right_tex_coords = ImVec2(ent_sprite->tex_coords[2], ent_sprite->tex_coords[3]);
				ImVec2 bottom_left_tex_coords = ImVec2(ent_sprite->tex_coords[6], ent_sprite->tex_coords[7]);
				ImVec2 button_size = ImVec2(32, 32);
				ImGui::PushID(indx);
				if (ImGui::ImageButton((ImTextureID)ent_sprite->atlas->handle, 
										button_size,
										bottom_left_tex_coords, top_right_tex_coords))
				{
					draggable_entity = Basic_Tile::create(template_tile->lua_id);
					id_selected_entity = template_tile->lua_id;
					editing_state = INSERT;
				}
				ImGui::PopID();
				bool is_end_of_row = !((indx + 1) % 6);
				bool is_last_element = indx == (template_tiles.size() - 1);
				if (!is_end_of_row && !is_last_element) { ImGui::SameLine(); }
			}
		}

		if (!draggable_entity) { editing_state = IDLE;  }
		if (editing_state == INSERT) {
			// Add a new entity to the tilemap on click
			if (game_input.is_down[GLFW_MOUSE_BUTTON_LEFT]) {
				glm::ivec2 grid_pos = grid_pos_from_px_pos(game_input.px_pos);
				Entity* current_entity = layers[indx_active_layer]->tiles[grid_pos.x][grid_pos.y];

				// We don't want to double paint, so check to make sure we're not doing that
				bool okay_to_create = true;

				// If the tile we're painting over exists and is the same kind we're trying to paint, don't do it
				if (current_entity) {
					if (current_entity->lua_id == draggable_entity->lua_id) {
						okay_to_create = false;
					}
				}

				if (okay_to_create) {
					// Create a lambda which will undo the tile placement we're about to do
					auto my_lambda = [map = layers[indx_active_layer],
						x = grid_pos.x, y = grid_pos.y,
						ent = layers[indx_active_layer]->tiles[grid_pos.x][grid_pos.y]]()
					{
						map->tiles[x][y] = ent;
					};
					stack.push_back(my_lambda);

					// Grab the translation from the mouse position and add the tile to tilemap
					SRT transform = srt_from_grid_pos(grid_pos_from_px_pos(game_input.px_pos));
					Position_Component* pc = draggable_entity->get_component<Position_Component>();
					pc->transform.translate = transform.translate;
					layers[indx_active_layer]->tiles[grid_pos.x][grid_pos.y] = draggable_entity;
					draggable_entity = Basic_Tile::create(id_selected_entity);

					// Update so we only paint one entity per tile
					last_grid_pos_drawn = grid_pos;
				}
			}

			// Correctly translate current selected tile to be under the mouse & scale it otherwise
			Position_Component* position_component = draggable_entity->get_component<Position_Component>();
			if (position_component) {
				Graphic_Component* graphic_component = draggable_entity->get_component<Graphic_Component>();
				if (graphic_component) {
					glm::ivec2 grid_pos = grid_pos_from_px_pos(game_input.px_pos);
					SRT transform = srt_from_grid_pos(grid_pos_from_px_pos(game_input.px_pos));
					position_component->transform.translate = transform.translate;
				}
			}
		}
		
		ImGui::End();
	}

	void render() {
		static bool* show_layers = (bool*)calloc(sizeof(bool), 128); // @hack; // For ImGui checkboxes to show layers
		
		// Render the layer selector and chosen layers
		ImGui::Begin("Tile Editor", 0, 0);
		if (ImGui::CollapsingHeader("Layers")) {
			ImGui::Text("Editing layer: %s", layers[indx_active_layer]->name.c_str());
			fox_for(indx_layer, layers.size()) {
				ImGui::Checkbox(layers[indx_layer]->name.c_str(), &show_layers[indx_layer]);
				if (show_layers[indx_layer]) { layers[indx_layer]->draw(); }
			}
		}
			
		if (draggable_entity) { draggable_entity->draw(); }

		// Actually make the draw calls to render all the tiles. Anything after this gets painted over it
		renderer.render_for_frame();

		// Render the grid
		{
			static bool show_grid;
			ImGui::Checkbox("Show grid", &show_grid);
			if (show_grid) {
				// We have to multiply by two because OpenGL uses -1 to 1
				for (float col_offset = -1; col_offset <= 1; col_offset += GLSCR_TILESIZE_X) {
					draw_line_from_points(glm::vec2(col_offset, -1.f), glm::vec2(col_offset, 1.f), glm::vec4(.2f, .1f, .9f, 0.5f));
				}
				for (float row_offset = 1; row_offset >= -1; row_offset -= GLSCR_TILESIZE_Y) {
					draw_line_from_points(glm::vec2(-1.f, row_offset), glm::vec2(1.f, row_offset), glm::vec4(.2f, .1f, .9f, 0.5f));
				}
			}
		}

		// If we're in selection mode, paint outlines over all the tiles we can select
		static glm::ivec2 hovered = glm::vec2(0);
		static glm::vec4 hovered_color = green;
		if (editing_state == SELECT) {
			// If we change what tile we are hovering over, fade the new tile from green to red and the old tile from red to green
			glm::ivec2 this_frame_hovered = grid_pos_from_px_pos(game_input.px_pos);
			if (this_frame_hovered != hovered) {
				hovered_color = green;
				hovered = this_frame_hovered;
			}

			// Draw bounding boxes on all the tiles
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (auto& layer : layers) {
				fox_for(x, MAP_SIZE) {
					fox_for(y, MAP_SIZE) {
						Entity* tile = layer->tiles[x][y];
						if (tile) {
							Position_Component* pc = tile->get_component<Position_Component>();
							if (pc) {
								draw_square_outline(pc->transform, glm::vec4(0.f, 1.f, 0.f, 1.f));
							}
						}
					}
				}
			}

			// Draw the tile that's hovered 
			hovered_color = glm::mix(hovered_color, red, .1f);
			draw_square_outline(srt_from_grid_pos(hovered), hovered_color);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		ImGui::End();
	}
} game_layer;


// The console needs access to the game layer, so it can pass down commands to it
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

