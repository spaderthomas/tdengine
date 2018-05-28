struct {
	int indx_active_tile = -1;
	enum {
		FREE,
		TILE_EDITING,
	} mode;
	glm::ivec2 top_left_tile_index;
	Tree* tree;
	Tilemap t;

	void init() {
		mode = TILE_EDITING;
		top_left_tile_index = glm::ivec2(0);
		Entity* (*create_func)() = (Entity* (*)())create_methods[typeid(Tree).name()];
		Entity* what = create_func();
		tree = dynamic_cast<Tree*>(what);
		t.name = "level_one";
		t.load();
	}

	void update(float dt) {
		if (mode == TILE_EDITING) {
			if (game_input.was_pressed(GLFW_KEY_UP)) {
				camera_pos.y -= GLSCR_TILESIZE_Y;
				top_left_tile_index.y -= 1;
			}
			if (game_input.was_pressed(GLFW_KEY_DOWN)) {
				camera_pos.y += GLSCR_TILESIZE_Y;
				top_left_tile_index.y += 1;
			}
			if (game_input.was_pressed(GLFW_KEY_RIGHT)) {
				camera_pos.x -= GLSCR_TILESIZE_X;
				top_left_tile_index.x += 1;
			}
			if (game_input.was_pressed(GLFW_KEY_LEFT)) {
				camera_pos.x += GLSCR_TILESIZE_X;
				top_left_tile_index.x -= 1;
			}

			if (game_input.was_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
				t.load();
			}
		}
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

		// Render the currently chosen entity
		//t.draw();
		Sprite* my_sprite = asset_table.get_asset<Sprite>("boon_walk1.png");
		ImGuiIO& io = ImGui::GetIO();
		ImTextureID my_tex_id = io.Fonts->TexID;
		float my_tex_w = (float)io.Fonts->TexWidth;
		float my_tex_h = (float)io.Fonts->TexHeight;

		fox_for(ient, template_entities.size()) {
			Entity* ent = template_entities[ient];
			Graphic_Component* gc = ent->get_component<Graphic_Component>();
			if (gc) {
				Sprite* my_sprite = gc->get_current_frame();
				ImGui::ImageButton((ImTextureID)my_sprite->atlas->handle, ImVec2(32, 32),
					ImVec2(my_sprite->tex_coords[0], my_sprite->tex_coords[1]), ImVec2(my_sprite->tex_coords[4], my_sprite->tex_coords[5]));
			}
		}
		tree->draw();
		renderer.render_for_frame();
	}
} game_layer;
