struct Set_State_Action : Action {
	string var;
	bool value;

	bool update(float dt) override;
	string kind() override { return "Set_State_Action"; }
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
