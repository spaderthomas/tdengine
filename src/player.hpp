struct Player {
	Entity_Visible* animator;
	SRT transform;
	
	float move_speed_x = GLSCR_TILESIZE_X * seconds_per_update * 4;
	float move_speed_y = GLSCR_TILESIZE_Y * seconds_per_update * 4;

	void init() {
		animator = entity_table.get_entity("boon");
		transform = SRT::no_transform();
		transform.scale = glm::vec2(GLSCR_TILESIZE_X, GLSCR_TILESIZE_Y);
		transform.translate = glm::vec2(-1.f + GLSCR_TILESIZE_X, 1.f - GLSCR_TILESIZE_Y); // To tile-align the player
	}
	void update(float dt) {
		// To handle window resizing
		transform.scale = glm::vec2(GLSCR_TILESIZE_X, GLSCR_TILESIZE_Y);
		animator->update(dt);
	}
	void bind() { animator->bind(); }
	void draw(GLuint mode) { animator->draw(mode); }
};