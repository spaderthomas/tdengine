struct Movement_Component : Component {
	glm::vec2 speed;
	glm::vec2 wish;
	void init(TableNode* table) override;
	string name() override;
	void imgui_visualizer() override;
};
