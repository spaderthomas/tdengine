struct Vision_Component : Component {
	float width;
	float depth;
	
	void init(TableNode* table) override;
	string name() override;
	void imgui_visualizer() override;
};
