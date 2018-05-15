struct Entity {
	string id;

	virtual void draw(SRT transform) {};
	virtual void draw(glm::ivec2 grid_pos) {};
};
struct Player : Entity {
	Graphic_Component* graphic_component;
	SRT transform;
	
	float move_speed_x = GLSCR_TILESIZE_X * seconds_per_update * 4;
	float move_speed_y = GLSCR_TILESIZE_Y * seconds_per_update * 4;

	void init() {
		graphic_component = new Graphic_Component;
		transform = SRT::no_transform();
		transform.scale = glm::vec2(GLSCR_TILESIZE_X, GLSCR_TILESIZE_Y);
	}
	void update(float dt) {
		// To handle window resizing
		transform.scale = glm::vec2(GLSCR_TILESIZE_X, GLSCR_TILESIZE_Y);
		graphic_component->update(dt);
	}
	
	void draw(SRT transform) override { graphic_component->draw(transform); }
	void draw(glm::ivec2 grid_pos) override { graphic_component->draw(grid_pos); }
};

struct Background_Tile : Entity {
	Graphic_Component* graphic_component;
	
	void init() {
		graphic_component = new Graphic_Component;
	}
	void draw(SRT transform) override { graphic_component->draw(transform); }
	void draw(glm::ivec2 grid_pos) override { graphic_component->draw(grid_pos); }
};
	
