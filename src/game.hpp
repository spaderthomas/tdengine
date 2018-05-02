struct {
	Player player;
	vector<glm::ivec2> filled_tiles;

	void init() {
		player.init();
		player.animator->start_animation("boon_walk");
	}

	void update(float dt) {
		if (game_input.is_down[TDNS_KEY_UP]) { player.transform.translate.y += player.move_speed_y; }
		if (game_input.is_down[TDNS_KEY_DOWN]) { player.transform.translate.y -= player.move_speed_y; }
		if (game_input.is_down[TDNS_KEY_RIGHT]) { player.transform.translate.x += player.move_speed_x; }
		if (game_input.is_down[TDNS_KEY_LEFT]) { player.transform.translate.x -= player.move_speed_x; }

		if (game_input.was_pressed(TDNS_MOUSE_LEFT)) { filled_tiles.push_back(game_input.grid_pos()); }

		player.update(dt);
	}

	void render() {
		// We have to multiply by two because OpenGL uses -1 to 1
		for (float col_offset = -1; col_offset <= 1; col_offset += GLSCR_TILESIZE_X) {
			draw_line_from_points(glm::vec2(col_offset, -1.f), glm::vec2(col_offset, 1.f), glm::vec4(.2f, .1f, .9f, 1.f));
		}
		for (float row_offset = 1; row_offset >= -1; row_offset -= GLSCR_TILESIZE_Y) {
			draw_line_from_points(glm::vec2(-1.f, row_offset), glm::vec2(1.f, row_offset), glm::vec4(.2f, .1f, .9f, 1.f));
		}

		for (auto& tile : filled_tiles) {
			draw_square(srt_from_grid_pos(tile), glm::vec4(.46f, .21f, .13f, 1.f));
		}
		auto grid_pos_transform = srt_from_grid_pos(game_input.grid_pos());
		draw_square(grid_pos_transform, glm::vec4(0.f, .4f, .2f, 1.f));


		textured_shader.bind();
		player.bind();
		textured_shader.set_int("sampler1", 0);
		auto transform_mat = mat3_from_transform(player.transform);
		textured_shader.set_mat3("transform", transform_mat);
		player.draw(GL_TRIANGLES);
	}
} game_layer;