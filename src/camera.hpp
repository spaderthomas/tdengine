/*
World space refers to the actual location of the thing in the world. 
*/
struct {
	glm::vec2 offset;
	EntityHandle following = {-1, nullptr};
} camera;

void camera_follow(EntityHandle entity) {
	camera.following = entity;
}
