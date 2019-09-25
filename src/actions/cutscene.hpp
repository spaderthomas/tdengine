struct Cutscene_Action : Action {
	string which;
	
	bool update(float dt) override;
	string kind() override { return "Cutscene_Action"; };
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
