struct Tile_Component : Component {
	int x;
	int y;
	
	std::string name() override;
	TableNode* save() const override;
	void load(TableNode* self);
	void imgui_visualizer() override;
};
