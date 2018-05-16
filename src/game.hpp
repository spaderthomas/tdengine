struct {
	Player* player;
	int indx_active_tile = -1;
	Tilemap level;
	enum {
		FREE,
		TILE_EDITING,
	} mode;
	glm::ivec2 top_left_tile_index;

	void init() {
		player = (Player*)entity_table.get_entity("boon");
		level.tiles[2][1] = entity_table.get_entity("grass");
		mode = TILE_EDITING;
		top_left_tile_index = glm::ivec2(0);
	}

	void update(float dt) {
		if (mode == FREE) {
			if (game_input.is_down[TDNS_KEY_UP]) {
				player->transform.translate.y += player->move_speed_y;
				camera_pos.y -= player->move_speed_y;
			}
			if (game_input.is_down[TDNS_KEY_DOWN]) {
				player->transform.translate.y -= player->move_speed_y;
				camera_pos.y += player->move_speed_y;
			}
			if (game_input.is_down[TDNS_KEY_RIGHT]) {
				player->transform.translate.x += player->move_speed_x;
				camera_pos.x -= player->move_speed_x;
			}
			if (game_input.is_down[TDNS_KEY_LEFT]) {
				player->transform.translate.x -= player->move_speed_x;
				camera_pos.x += player->move_speed_x;
			}
			if (game_input.was_pressed(TDNS_KEY_T)) {
			}
			
			if (camera_pos.x > 0) { camera_pos.x = 0; }
			if (camera_pos.y > 0) { camera_pos.y = 0; }
			player->update(dt);
		}
		else if (mode == TILE_EDITING) {
			if (game_input.was_pressed(TDNS_KEY_UP)) {
				camera_pos.y -= GLSCR_TILESIZE_Y;
				top_left_tile_index.y -= 1;
			}
			if (game_input.was_pressed(TDNS_KEY_DOWN)) {
				camera_pos.y += GLSCR_TILESIZE_Y;
				top_left_tile_index.y += 1;
			}
			if (game_input.was_pressed(TDNS_KEY_RIGHT)) {
				camera_pos.x -= GLSCR_TILESIZE_X;
				top_left_tile_index.x += 1;
			}
			if (game_input.was_pressed(TDNS_KEY_LEFT)) {
				camera_pos.x += GLSCR_TILESIZE_X;
				top_left_tile_index.x -= 1;
			}
			if (game_input.is_down[TDNS_MOUSE_LEFT]) {
				if (indx_active_tile != -1) {
					auto screen_grid_pos = grid_pos_from_px_pos(game_input.px_pos);
					auto tilemap_pos = glm::ivec2(screen_grid_pos.x + top_left_tile_index.x, screen_grid_pos.y + top_left_tile_index.y);
					if (tilemap_pos.x > -1 && tilemap_pos.y < 128 && tilemap_pos.y > -1 && tilemap_pos.y < 128) {
						level.tiles[tilemap_pos.x][tilemap_pos.y] = entity_table.entities[indx_active_tile];
					}
				}
			}
			if (game_input.was_pressed(TDNS_KEY_S)) { level.save(); }
			if (game_input.was_pressed(TDNS_KEY_W)) { level.load(); }

			if (game_input.was_pressed(TDNS_KEY_TAB)) {
				indx_active_tile = (indx_active_tile + 1) % entity_table.entities.size();
				if (player == entity_table.get_entity("boon")) {
					player = (Player*)entity_table.get_entity("wilson");
					player->graphic_component->start_animation("wilson_walk");
				}
				else {
					player = (Player*)entity_table.get_entity("boon");
					player->graphic_component->start_animation("boon_walk");
				}
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
		if (indx_active_tile != -1) {
			Background_Tile* active_tile = (Background_Tile*)entity_table.entities[indx_active_tile];
			SRT transform = srt_from_grid_pos(grid_pos_from_px_pos(game_input.px_pos));
			transform.scale = active_tile->graphic_component->get_scaling();
			transform.translate -= glm::vec2(camera_pos.x, camera_pos.y);
			active_tile->draw(transform);
		}

		player->draw(player->transform);
		level.draw();
		renderer.render_for_frame();

	}
} game_layer;
