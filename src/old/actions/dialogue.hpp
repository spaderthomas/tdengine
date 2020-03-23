struct Dialogue_Action : Action {
	Dialogue_Tree* tree;
	bool update(float dt) override;
	std::string kind() override { return "Dialogue_Action"; }
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
