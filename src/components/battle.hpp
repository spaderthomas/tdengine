struct Battle_Component : Component {
	unsigned health;
	vector<Move*> moves;
	
	void init_from_table(TableNode* table);
	string name() override;
	void imgui_visualizer() override;
};
