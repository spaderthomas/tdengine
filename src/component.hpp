struct Component {
	virtual TableNode* Component::save() const { return nullptr; }
	virtual void init(TableNode* table) {};
	virtual void load(TableNode* table) { cout << "Tried to call virtual load() on base Component"; };
	virtual string name() { return "Component"; };
	virtual void imgui_visualizer() = 0;
};


