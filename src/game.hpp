struct {
	Player* player;
	int indx_active_tile = -1;
	Tilemap level;

	void init() {
		player = (Player*)entity_table.get_entity("boon");
		level.tiles[2][1] = entity_table.get_entity("grass");

	}

	void update(float dt) {
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

		if (game_input.is_down[TDNS_MOUSE_LEFT]) {
			if (indx_active_tile != -1) {
				auto grid_pos = game_input.grid_pos();
				if (indx_active_tile != -1) {
					level.tiles[grid_pos.x][grid_pos.y] = entity_table.entities[indx_active_tile];
				}
			}
		}

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

		player->update(dt);
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
			SRT transform = srt_from_grid_pos(game_input.grid_pos());
			transform.scale = active_tile->graphic_component->get_scaling();
			transform.translate -= glm::vec2(camera_pos.x, camera_pos.y);
			active_tile->draw(transform);
		}

		player->draw(player->transform);
		level.draw();
		renderer.render_for_frame();

	}
} game_layer;
