struct Component {
	virtual TableNode* save() const { return nullptr; }
	virtual TableNode* make_template() const { return new TableNode; }
	
	virtual void init(TableNode* table) {};
	virtual void load(TableNode* table) { std::cout << "Tried to call virtual load() on base Component"; };
	virtual std::string name() { return "Component"; };
	virtual void imgui_visualizer() = 0;
};
