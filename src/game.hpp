void __stdcall dbGLDebugMessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *userParam) {
	(void)userParam;

	switch (id) {
	case 131169: // The driver allocated storage for renderbuffer
		return;
	case 131185: // glBufferData
		return;
	case 481131: // buffer info
		return;
	case 131184: // buffer info
		return;
	}

	string debug_msg;
	debug_msg += "OpenGL Debug Message: ";
	debug_msg += "\nSource: ";
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		write_str(debug_msg, "API");
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		write_str(debug_msg, "Window System");
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		write_str(debug_msg, "Shader Compiler");
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		write_str(debug_msg, "Third Party");
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		write_str(debug_msg, "Application");
		break;
	case GL_DEBUG_SOURCE_OTHER:
		write_str(debug_msg, "Other");
		break;
	}

	write_str(debug_msg, "\nType: ");
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		write_str(debug_msg, "Error");
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		write_str(debug_msg, "Deprecated Behaviour");
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		write_str(debug_msg, "Undefined Behaviour");
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		write_str(debug_msg, "Portability");
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		write_str(debug_msg, "Performance");
		break;
	case GL_DEBUG_TYPE_MARKER:
		write_str(debug_msg, "Marker");
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		write_str(debug_msg, "Push Group");
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		write_str(debug_msg, "Pop Group");
		break;
	case GL_DEBUG_TYPE_OTHER:
		write_str(debug_msg, "Other");
		break;
	}

	write_str(debug_msg, "\nID: ");
	write_uint(debug_msg, id);

	write_str(debug_msg, "\nSeverity: ");
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		write_str(debug_msg, "High");
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		write_str(debug_msg, "Medium");
		break;
	case GL_DEBUG_SEVERITY_LOW:
		write_str(debug_msg, "Low");
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		write_str(debug_msg, "Notification");
		break;
	}

	write_str(debug_msg, "\n\n");
	game_state->log.write_str(const_str(debug_msg.elems));
	game_state->heap_stack.restore(ta);
}

struct Character {
	GLuint texture;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLuint advance;
};
map<GLchar, Character> characters;

FT_Library freetype;
FT_Face face;
GLuint ft_vao, ft_vert_buffer;

void init_freetype() {
	if (FT_Init_FreeType(&freetype)) {
		tdns_log.write("Failed to initialize FreeType");
		exit(0);
	}
	if (FT_New_Face(freetype, "C:/Windows/Fonts/PxPlus_IBM_VGA8.ttf", 0, &face)) {
		tdns_log.write("Failed to load font");
		exit(0);
	}

	FT_Set_Pixel_Sizes(face, 0, 16);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			tdns_log.write("FreeType failed to load character");
			tdns_log.write(string("Character was: %c", c));
			exit(0);
		}

		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters.insert(std::pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(freetype);

	glGenVertexArrays(1, &ft_vao);
	glGenBuffers(1, &ft_vert_buffer);
	glBindVertexArray(ft_vao);
	glBindBuffer(GL_ARRAY_BUFFER, ft_vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void render_text(float x, float y, float scale, glm::vec3 color, string text) {
	text_shader.bind();
	text_shader.set_vec3("text_color", color);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_X), 0.0f, static_cast<GLfloat>(SCREEN_Y));
	text_shader.set_mat4("projection", projection);
	glBindVertexArray(ft_vao);

	for (auto it = text.begin(); it != text.end(); it++) {
		Character freetype_char = characters[*it];

		GLfloat xpos = x + freetype_char.bearing.x * scale;
		GLfloat ypos = y - (freetype_char.size.y - freetype_char.bearing.y) * scale;

		GLfloat w = freetype_char.size.x * scale;
		GLfloat h = freetype_char.size.y * scale;
		
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, freetype_char.texture);
		glBindBuffer(GL_ARRAY_BUFFER, ft_vert_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (freetype_char.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
}


struct Entity_Tree {
	string dir;
	vector<Entity*> entities;
	vector<Entity_Tree*> children;

	int size;

	static Entity_Tree* create(string dir) {
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

	Entity* find(string lua_id) {
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
}; 

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
	Entity_Tree* tile_tree;
	Entity_Tree* entity_tree;
	glm::ivec2 last_grid_pos_drawn;
	enum {
		IDLE,
		INSERT,
		SELECT
	} editing_state;
	Entity* selected = nullptr;
	string id_selected_entity;
	bool show_grid;
	bool snap_to_grid;

	Level dude_ranch;

	Console console;

	void exec_console_command(const char* command_line) {
		if (console.Stricmp(command_line, "SAVE") == 0) {
			dude_ranch.save();
		}
		else if (console.Stricmp(command_line, "LOAD") == 0) {
			dude_ranch.load();
		}
		else if (console.Stricmp(command_line, "RELOAD") == 0) {
			//@leak this one is quite big 
			for (auto dirname : atlas_folders) {
				create_texture_atlas(dirname);
			}

			Lua.load_scripts();

			tile_tree = Entity_Tree::create("..\\..\\textures\\tiles");
			entity_tree = Entity_Tree::create("..\\..\\textures\\entities");

			// Reload all graphics components
			for (auto it : dude_ranch.chunks) {
				Chunk& chunk = it.second;
				fox_for(x, CHUNK_SIZE) {
					fox_for(y, CHUNK_SIZE) {
						Entity* cur = chunk.tiles[x][y];
						if (cur != nullptr) {
							Graphic_Component* gc = cur->get_component<Graphic_Component>();
							if (gc) {
								gc->init_from_table(Lua.state[cur->lua_id]["Graphic_Component"]);
							}
						}
					}
				}
			}
		}
		else {
			console.AddLog("Unknown command: '%s'. Loading up Celery Man instead.\n", command_line);
		}
	}

	void init() {
		editing_state = IDLE;
		tile_tree = Entity_Tree::create("..\\..\\textures\\tiles");
		entity_tree = Entity_Tree::create("..\\..\\textures\\entities");
		dude_ranch.name = "dude_ranch";
		init_freetype();
		create_texture("..\\..\\textures\\reference\\lone_star.png");
	}
	
	void update(float dt) {
		if (game_input.is_down[GLFW_KEY_UP]) {
			camera_top_left.y = fox_max(0, camera_top_left.y - 1);
		}
		if (game_input.is_down[GLFW_KEY_DOWN]) {
			camera_top_left.y = camera_top_left.y + 1;
		}
		if (game_input.is_down[GLFW_KEY_RIGHT]) {
				camera_top_left.x += 1;
			}
		if (game_input.is_down[GLFW_KEY_LEFT]) {
			camera_top_left.x = fox_max(0, camera_top_left.x - 1);
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
			selected = nullptr;
			editing_state = IDLE;
		}
		if (game_input.was_pressed(GLFW_KEY_0)) {
			editing_state = SELECT;
		}
		
		// Toggle the console on control
		static bool show_console = false;
		static bool console_close = false;
		if (global_input.was_pressed(GLFW_KEY_LEFT_CONTROL)) {
			show_console = !show_console;
		}
		if (show_console) {
			console.Draw("tdnsconsole", &console_close);
		}

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::Begin("Level Editor", 0, flags);
		
		if (ImGui::CollapsingHeader("Options")) {
			ImGui::Checkbox("Show grid", &show_grid);
			ImGui::Checkbox("Snap to grid", &snap_to_grid);
		}

		// Tile Selector GUI
		int unique_button_index = 0; // Needed for ImGui
		
		auto draw_buttons = [&](Entity_Tree* root) -> void {
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
								selected = Entity::create(tile->lua_id);
								id_selected_entity = tile->lua_id;
								editing_state = INSERT;
							}
							ImGui::PopID();
							bool is_end_of_row = !((indx + 1) % 6);
							bool is_last_element = indx == (tile_tree->size - 1);
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
			lambda(root, lambda);
		};
		draw_buttons(tile_tree);
		draw_buttons(entity_tree);

		//@hack
		if (!selected) { editing_state = IDLE;  }

		// INSERT MODE: There is a selected tile or entity, and we can move it around and click to place it.
		if (editing_state == INSERT) {
			static int lag_frames; // so we dont paint fifty entities every time we click

			if (game_input.is_down[GLFW_MOUSE_BUTTON_LEFT]) {
				glm::vec3 translation;
				glm::ivec2 grid_pos = grid_pos_from_px_pos(game_input.px_pos) + camera_top_left;
				if (snap_to_grid) { translation = translation_from_grid_pos(grid_pos); }
				else { translation = translation_from_px_pos(game_input.px_pos); }

				// Add a grid tile
				if (tile_tree->find(selected->lua_id)) {
					Entity* current_entity = dude_ranch.get_tile(grid_pos.x, grid_pos.y);

					// We don't want to double paint, so check to make sure we're not doing that
					bool okay_to_create = true;

					// If the tile we're painting over exists and is the same kind we're trying to paint, don't do it
					if (current_entity) {
						if (current_entity->lua_id == selected->lua_id) {
							okay_to_create = false;
						}
					}

					if (okay_to_create) {
						// Create a lambda which will undo the tile placement we're about to do
						auto my_lambda = [&dude_ranch = dude_ranch,
							x = grid_pos.x, y = grid_pos.y,
							ent = dude_ranch.get_tile(grid_pos.x, grid_pos.y)]
						{
							dude_ranch.set_tile(ent, x, y);
						};
						stack.push_back(my_lambda);

						// Grab the translation from the mouse position and figure out whether its a tile or an entity
						Entity* new_entity = Entity::create(selected->lua_id);
						

						Position_Component* pc = new_entity->get_component<Position_Component>();
						pc->transform.translate = translation;
						dude_ranch.set_tile(new_entity, grid_pos.x, grid_pos.y);

						// Update so we only paint one entity per tile
						last_grid_pos_drawn = grid_pos;
					}
				}
				// Add an entity
				else if (entity_tree->find(selected->lua_id)) {
					if (lag_frames == 0) {
						auto my_lambda = [&dude_ranch = dude_ranch]() -> void {
							dude_ranch.entities.pop_back();
						};
						stack.push_back(my_lambda);

						Entity* new_entity = Entity::create(selected->lua_id);
						Position_Component* pc = new_entity->get_component<Position_Component>();
						pc->transform.translate = translation;
						dude_ranch.entities.push_back(new_entity);
						lag_frames = 30;
					}
				}
			}

			lag_frames = fox_max(0, lag_frames - 1);

			// Correctly translate current selected tile to be under the mouse & scale it otherwise
			Position_Component* pc = selected->get_component<Position_Component>();
			if (pc) {
				glm::ivec2 grid_pos = grid_pos_from_px_pos(game_input.px_pos) + camera_top_left;
				if (snap_to_grid) {
					pc->transform.translate = translation_from_grid_pos(grid_pos);
				}
				else {
					pc->transform.translate = translation_from_px_pos(game_input.px_pos);
				}
			}
		}
		
		ImGui::End();
	}

	void render() {
		static bool* show_layers = (bool*)calloc(sizeof(bool), 128); // @hack; // For ImGui checkboxes to show layers
		
		// Render the layer selector and chosen layers
		ImGui::Begin("Level Editor", 0, 0);
		
		dude_ranch.draw();
		if (selected) { selected->draw(); }

		// Actually make the draw calls to render all the tiles. Anything after this gets painted over it
		renderer.render_for_frame();

		// Render the grid
		{
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

		// SELECT MODE: Lets you select previously placed tiles and edit them. Render a fade-in outline around selected tile.
		static glm::ivec2 hovered = glm::vec2(0);
		static glm::vec4 hovered_color = green;
		if (editing_state == SELECT) {
			// If we change what tile we are hovering over, fade the new tile from green to red and the old tile from red to green
			glm::ivec2 this_frame_hovered = grid_pos_from_px_pos(game_input.px_pos);
			if (this_frame_hovered != hovered) {
				hovered_color = green;
				hovered = this_frame_hovered;
			}

			// Draw the tile that's hovered 
			hovered_color = glm::mix(hovered_color, red, .1f);
			draw_square_outline(srt_from_grid_pos(glm::ivec2(5, 5)), blue);
			draw_square_outline(srt_from_grid_pos(hovered), hovered_color);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		ImGui::End();


#if 0
		SRT transform = SRT::no_transform();
		glm::vec2 bottom_left = glm::vec2(-.9, -.9);
		transform.scale = glm::vec2(-1.f * bottom_left.x, .25f);
		transform.translate = glm::vec3(bottom_left.x + transform.scale.x, bottom_left.y + transform.scale.y, 1.f);
		draw_square(transform, red);

		draw_rectangle(glm::vec2(0, 0), glm::vec2(.25, .25), hannah_color);
#endif
		SRT transform = SRT::no_transform();
		glm::vec2 bottom_left = glm::vec2(-.9, -.9);
		transform.scale = glm::vec2(-1.f * bottom_left.x, .25f);
		transform.translate = glm::vec3(bottom_left.x + transform.scale.x, bottom_left.y + transform.scale.y, 1.f);
		textured_shader.bind();
		textured_shader.set_int("sampler", 0);
		glm::vec2 camera_pos = glm::vec2(0.f, 0.f);
		textured_shader.set_vec2("camera_pos", camera_pos);
		auto texture = asset_table.get_asset<Texture>("lone_star.png");
		bind_sprite_buffers();
		glBindTexture(GL_TEXTURE_2D, texture->handle);

		// 0: vertices, 1: texcoords
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), square_tex_coords_offset);
		glEnableVertexAttribArray(1);

		auto transform_mat = mat3_from_transform(transform);
		textured_shader.set_mat3("transform", transform_mat);
		textured_shader.set_int("z", 1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//stbtt_print(0, 0, "C");
		render_text(100, 100, 1, blue, "whoa would you look at that that sure is a lot of text right there");

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

