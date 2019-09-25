struct Camera_Pan_Action : Action {
	glm::vec2 dest;
	int count_frames;
	int frames_elapsed = 0; 

	bool update(float dt) override;
	string kind() override { return "Camera_Pan_Action"; };
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
