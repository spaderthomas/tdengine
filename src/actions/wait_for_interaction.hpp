struct Wait_For_Interaction_Action : Action {
	bool update(float dt) override;
	string kind() override { return "Wait_For_Interaction_Action"; }
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
