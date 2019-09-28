struct Graphic_Component : Component {
	vector<Animation*> animations;
	Animation* active_animation = nullptr;
	glm::vec2 scale;
	int z;

	void begin_animation(string wish);
	void add_animation(Animation* anim);
	Sprite* get_current_frame();
	void init_from_table(TableNode* table);
	string name() override;
	void imgui_visualizer() override;
};
