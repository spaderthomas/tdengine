struct Interaction_Component : Component {
	bool was_interacted_with = false;
	EntityHandle other;
	
	string name() override;
	void imgui_visualizer() override;
};
