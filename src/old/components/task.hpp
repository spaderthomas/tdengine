struct Task_Component : Component {
	Task* task;
	
	std::string name() override;
	void init(TableNode* table) override;
	void imgui_visualizer() override;
};
