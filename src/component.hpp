struct Component {
	virtual TableNode* save() const;
	virtual void load(TableNode* table) { cout << "Tried to call virtual load() on base Component"; };
	virtual string name() { return "Component"; };
	virtual void imgui_visualizer() = 0;

	// @spader 8/23/2019: Make init_from_table virtual
};


