struct {

} undo_stack;

vector<function<void()>> stack;
struct {
	vector<Tilemap*> layers;
	int indx_active_layer = -1;
	glm::ivec2 last_grid_pos_drawn;
	enum {
		IDLE,
		CREATING_ENTITY
	} editing_state;
	Entity* draggable_entity = nullptr;
	string id_selected_entity;

	void init() {
		editing_state = IDLE;
		layers.push_back(new Tilemap);
		layers.push_back(new Tilemap);
		layers[0]->name = "layer_one";
		layers[1]->name = "layer_two";
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
		if (game_input.was_pressed(GLFW_KEY_S)) {
			layers[indx_active_layer]->save();
		}
		if (game_input.was_pressed(GLFW_KEY_L)) {
			layers[indx_active_layer]->load();
		}
		if (game_input.is_down[GLFW_KEY_LEFT_CONTROL] &&
			game_input.was_pressed(GLFW_KEY_Z)) 
		{
			if (stack.size()) {
				function<void()> undo = stack.back();
				stack.pop_back();
				undo();
			}
		}
		if (game_input.was_pressed(GLFW_KEY_R)) {
			// Reload all graphics components
			fox_for(x, MAP_SIZE) {
				fox_for(y, MAP_SIZE) {
					Entity* cur = layers[indx_active_layer]->tiles[x][y];
					Graphic_Component* gc = cur->get_component<Graphic_Component>();
					if (gc) {
						gc->load_animations_from_lua(Lua.state[cur->lua_id]["Graphic_Component"]);
					}
				}
			}
		}

		// Tile Selector GUI
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::Begin("Tile Editor", 0, flags);
		ImGui::Text("Layer %i of %i", indx_active_layer + 1, layers.size());
		fox_for(indx, template_tiles.size()) {
			Entity* template_tile = template_tiles[indx];
			Graphic_Component* gc = template_tile->get_component<Graphic_Component>();
			if (gc) {
				Sprite* ent_sprite = gc->get_current_frame();

 				ImVec2 top_right_tex_coords = ImVec2(ent_sprite->tex_coords[0], ent_sprite->tex_coords[1]);
				ImVec2 bottom_left_tex_coords = ImVec2(ent_sprite->tex_coords[4], ent_sprite->tex_coords[5]);
				ImVec2 button_size = ImVec2(32, 32);
				ImGui::PushID(indx);
				if (ImGui::ImageButton((ImTextureID)ent_sprite->atlas->handle, 
										button_size,
										top_right_tex_coords, bottom_left_tex_coords)) {
					draggable_entity = Basic_Tile::create(template_tile->lua_id);
					id_selected_entity = template_tile->lua_id;
					editing_state = CREATING_ENTITY;
				}
				ImGui::PopID();
				if ((indx+1) % 4) { ImGui::SameLine(); }
			}
		}

		if (editing_state == CREATING_ENTITY) {
			// Add a new entity to the tilemap!
			if (game_input.is_down[GLFW_MOUSE_BUTTON_LEFT]) {
				glm::ivec2 grid_pos = grid_pos_from_px_pos(game_input.px_pos);
				if (grid_pos.x < 0 || grid_pos.y < 0) {
					int x = 1;
				}
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

			// Correctly translate current selected tile to be under the mouse & scale it
			Position_Component* position_component = draggable_entity->get_component<Position_Component>();
			if (position_component) {
				Graphic_Component* graphic_component = draggable_entity->get_component<Graphic_Component>();
				if (graphic_component) {
					SRT transform = srt_from_grid_pos(grid_pos_from_px_pos(game_input.px_pos));
					position_component->transform.translate = transform.translate;
				}
			}
			
		}
		ImGui::End();
	}

	void render() {
		// Render the grid
		{
			// We have to multiply by two because OpenGL uses -1 to 1
			for (float col_offset = -1; col_offset <= 1; col_offset += GLSCR_TILESIZE_X) {
				draw_line_from_points(glm::vec2(col_offset, -1.f), glm::vec2(col_offset, 1.f), glm::vec4(.2f, .1f, .9f, 1.f));
			}
			for (float row_offset = 1; row_offset >= -1; row_offset -= GLSCR_TILESIZE_Y) {
				draw_line_from_points(glm::vec2(-1.f, row_offset), glm::vec2(1.f, row_offset), glm::vec4(.2f, .1f, .9f, 1.f));
			}
		}
		
		for (auto& layer : layers) { layer->draw(); }
		if (draggable_entity) { draggable_entity->draw(); }
		renderer.render_for_frame();
	}
} game_layer;
