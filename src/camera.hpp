struct {
	glm::vec2 offset; // This defines which tile is on the upper left of the screen
	EntityHandle following = { -1, nullptr };
} camera;
