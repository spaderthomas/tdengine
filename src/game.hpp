struct {
	Player player;
	vector<glm::ivec2> filled_tiles;
	vector<Entity_Visible*> tiles;
	int indx_active_tile = -1;

	void init() {
		player.init();
		player.animator->start_animation("boon_walk");
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
			if (player.animator == entity_table.get_entity("boon")) {
				player.animator = entity_table.get_entity("wilson");
				player.animator->start_animation("wilson_walk");
			}
			else {
				player.animator = entity_table.get_entity("boon");
				player.animator->start_animation("boon_walk");
			}

		}

		player.update(dt);
	}

	void render() {
		// Render the grid
		// We have to multiply by two because OpenGL uses -1 to 1
		for (float col_offset = -1; col_offset <= 1; col_offset += GLSCR_TILESIZE_X) {
			draw_line_from_points(glm::vec2(col_offset, -1.f), glm::vec2(col_offset, 1.f), glm::vec4(.2f, .1f, .9f, 1.f));
		}
		for (float row_offset = 1; row_offset >= -1; row_offset -= GLSCR_TILESIZE_Y) {
			draw_line_from_points(glm::vec2(-1.f, row_offset), glm::vec2(1.f, row_offset), glm::vec4(.2f, .1f, .9f, 1.f));
		}

		// Render the tile map
		fox_for (itile, filled_tiles.size()) {
			glm::ivec2& tile_pos = filled_tiles[itile];
			Entity_Visible* tile = tiles[itile];
			SRT transform = srt_from_grid_pos(tile_pos);
			transform.scale = glm::vec2(tile->tilesize.x * SCR_TILESIZE_X, tile->tilesize.y * SCR_TILESIZE_Y);
			tile->draw(GL_TRIANGLES, transform);
		}

		/*
		auto grid_pos_transform = srt_from_grid_pos(game_input.grid_pos());
		if (indx_active_tile != -1) {
			Entity_Visible* active_tile = entity_table.entities[indx_active_tile];
			grid_pos_transform.scale = glm::vec2(active_tile->tilesize.x * SCR_TILESIZE_X, active_tile->tilesize.y * SCR_TILESIZE_Y);
			auto transform_mat = mat3_from_transform(grid_pos_transform);
			textured_shader.bind();
			active_tile->bind();
			textured_shader.set_int("sampler1", 0);
			textured_shader.set_mat3("transform", transform_mat);
			active_tile->draw(GL_TRIANGLES);
		}
		*/

		player.draw(GL_TRIANGLES);
		renderer.render_for_frame();

	}
} game_layer;
