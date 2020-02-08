struct Door_Component : Component {
	std::string level;
	glm::vec2 position;
	
	void init(TableNode* table) override;
	TableNode* make_template() const override;
	TableNode* save() const override;
	void load(TableNode* self);
	std::string name() override;
	void imgui_visualizer() override;
};
