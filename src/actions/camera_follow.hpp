struct Camera_Follow_Action : Action {
	std::string who;
	bool pan;
	bool is_first_update = true;
	
	bool update(float dt) override;
	std::string kind() override { return "Camera_Follow_Action"; };
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
