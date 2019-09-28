struct Task_Component : Component {
	Task* task;
	
	string name() override;
	void init_from_table(TableNode* table);
	void imgui_visualizer() override;
};
