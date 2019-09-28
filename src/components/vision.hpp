struct Vision_Component : Component {
	float width;
	float depth;
	
	void init_from_table(TableNode* table);
	string name() override;
	void imgui_visualizer() override;
};
