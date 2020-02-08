struct Set_State_Action : Action {
	std::string var;
	bool value;

	bool update(float dt) override;
	std::string kind() override { return "Set_State_Action"; }
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
