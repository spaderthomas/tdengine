struct Task_Component : Component {
	Task* task;
	
	string name() override;
	void init(TableNode* table) override;
	void imgui_visualizer() override;
};
