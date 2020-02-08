struct Battle_Component : Component {
	int health;
	std::vector<Move*> moves;

	TableNode* make_template() const override;
	void init(TableNode* table) override;
	std::string name() override;
	void imgui_visualizer() override;
};
