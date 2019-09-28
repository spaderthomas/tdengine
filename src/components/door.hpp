struct Door_Component : Component {
	string level;
	glm::vec2 position;
	
	void init_from_table(TableNode* table);
	TableNode* save() const override;
	void load(TableNode* self);
	string name() override;
	void imgui_visualizer() override;
};
