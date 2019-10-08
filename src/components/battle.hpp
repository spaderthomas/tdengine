struct Battle_Component : Component {
	unsigned health;
	vector<Move*> moves;
	
	void init(TableNode* table) override;
	string name() override;
	void imgui_visualizer() override;
};
