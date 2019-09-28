struct Movement_Component : Component {
	glm::vec2 speed;
	glm::vec2 wish;
	void init_from_table(TableNode* table);
	string name() override;
	void imgui_visualizer() override;
};
