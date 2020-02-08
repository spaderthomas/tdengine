struct Interaction_Component : Component {
	bool was_interacted_with = false;
	EntityHandle other;
	
	std::string name() override;
	void imgui_visualizer() override;
};
