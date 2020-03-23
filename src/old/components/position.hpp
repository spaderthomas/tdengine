struct Position_Component : Component {
	glm::vec2 world_pos = glm::vec2(0.f);
	
	TableNode* save() const override;
	void load(TableNode* self) override;
	std::string name() override;
	void imgui_visualizer() override;
};
