struct Player {
	Entity_Visible* animator;
	SRT transform;
	
	float move_speed_x = GLSCR_TILESIZE_X;
	float move_speed_y = GLSCR_TILESIZE_Y;

	void init() {
		animator = octopus;
		transform = SRT::no_transform();
		transform.scale = glm::vec2(GLSCR_TILESIZE_X, GLSCR_TILESIZE_Y);
		transform.translate = glm::vec2(-1.f + GLSCR_TILESIZE_X, 1.f - GLSCR_TILESIZE_Y); // To tile-align the player
	}
	void update() {
		transform.scale = glm::vec2(GLSCR_TILESIZE_X, GLSCR_TILESIZE_Y);
	}
	void bind() { animator->bind(); }
	void draw(GLuint mode) { animator->draw(mode); }
};