struct Movement_Component : Component {
	glm::vec2 speed;
	glm::vec2 wish;

	TableNode* make_template() const override;
	void init(TableNode* table) override;
	std::string name() override;
	void imgui_visualizer() override;
};
