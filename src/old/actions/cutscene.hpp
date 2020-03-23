struct Cutscene_Action : Action {
	std::string which;
	
	bool update(float dt) override;
	std::string kind() override { return "Cutscene_Action"; };
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
