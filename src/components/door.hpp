struct Door_Component : Component {
	string level;
	glm::vec2 position;
	
	void init(TableNode* table) override;
	TableNode* save() const override;
	void load(TableNode* self);
	string name() override;
	void imgui_visualizer() override;
};
