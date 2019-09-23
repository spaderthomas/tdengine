#define CAMERA_ADJUST_AMOUNT .005f
struct Camera {
	glm::vec2 offset; // This defines the upper left of the screen
	EntityHandle following = NULL_ENTITY;
	bool panning = false;

	void update(float dt);
	void follow(EntityHandle entity);
	void pan_and_follow(EntityHandle entity);
	bool is_at_entity(EntityHandle entity);
};
