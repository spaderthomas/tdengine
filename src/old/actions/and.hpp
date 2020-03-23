// Conjunction of actions which blocks until all are complete
struct And_Action : Action {
	std::vector<Action*> actions;
	bool update(float dt) override;
	std::string kind() override { return "And_Action"; }
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
