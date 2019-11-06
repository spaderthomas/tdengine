struct Graphic_Component : Component {
	vector<Animation*> animations;
	Animation* active_animation = nullptr;
	glm::vec2 scale;
	int z;

	void add_animation(Animation* anim);
	void begin_animation(string wish);
	Sprite* get_current_frame();
	
	TableNode* make_template() const override;
	void init(TableNode* table) override;
	string name() override;
	void imgui_visualizer() override;
};
