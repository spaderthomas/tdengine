struct Movement_Action : Action {
	glm::vec2 dest;

	bool update(float dt) override;
	string kind() override { return "Movement_Action"; }
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
