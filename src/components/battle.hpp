struct Battle_Component : Component {
	int health;
	vector<Move*> moves;

	TableNode* make_template() const override;
	void init(TableNode* table) override;
	string name() override;
	void imgui_visualizer() override;
};
