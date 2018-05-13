struct {
	Player player;
	vector<glm::ivec2> filled_tiles;
	vector<Entity_Visible*> tiles;
	int indx_active_tile = -1;

	void init() {
		player.init();
		player.entity_visible->start_animation("boon_walk");
	}

	void update(float dt) {
		if (game_input.is_down[TDNS_KEY_UP]) { player.transform.translate.y += player.move_speed_y; }
		if (game_input.is_down[TDNS_KEY_DOWN]) { player.transform.translate.y -= player.move_speed_y; }
		if (game_input.is_down[TDNS_KEY_RIGHT]) { player.transform.translate.x += player.move_speed_x; }
		if (game_input.is_down[TDNS_KEY_LEFT]) { player.transform.translate.x -= player.move_speed_x; }

		if (game_input.is_down[TDNS_MOUSE_LEFT]) {
			if (indx_active_tile != -1) {
				auto grid_pos = game_input.grid_pos();
				auto iter = find(filled_tiles.begin(), filled_tiles.end(), grid_pos);
				if (iter == filled_tiles.end()) {
					filled_tiles.push_back(grid_pos);
					tiles.push_back(entity_table.entities[indx_active_tile]);
				} else {
					int index = iter - filled_tiles.begin();
					tiles[index] = entity_table.entities[indx_active_tile];
				}
			}
		}

		if (game_input.was_pressed(TDNS_KEY_TAB)) { 
			indx_active_tile = (indx_active_tile + 1) % entity_table.entities.size();
			if (player.entity_visible == entity_table.get_entity("boon")) {
				player.entity_visible = entity_table.get_entity("wilson");
				player.entity_visible->start_animation("wilson_walk");
			}
			else {
				player.entity_visible = entity_table.get_entity("boon");
				player.entity_visible->start_animation("boon_walk");
			}

		}

		player.update(dt);
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

		// Render the tile map
		fox_for (itile, filled_tiles.size()) {
			tiles[itile]->draw(filled_tiles[itile]);
		}

		// Render the currently chosen entity
		if (indx_active_tile != -1) {
			Entity_Visible* active_tile = entity_table.entities[indx_active_tile];
			active_tile->draw(game_input.grid_pos());
		}

		player.draw(GL_TRIANGLES);
		renderer.render_for_frame();

	}
} game_layer;
