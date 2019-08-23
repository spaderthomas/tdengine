struct Camera {
	glm::vec2 offset; // This defines the upper left of the screen
	EntityHandle following = NULL_ENTITY;

	void update(float dt);
};
