struct Collider {
	EntityHandle entity;
	glm::vec2 origin;
	glm::vec2 extents;
};

struct MoveRequest {
	Collider collider;
	glm::vec2 wish;
};

bool are_boxes_colliding(Center_Box a, Center_Box b, glm::vec2& penetration);

struct PhysicsEngine {
	std::vector<Collider> colliders;
	std::vector<MoveRequest> requests;
	
	void update(float dt);
};

PhysicsEngine& get_physics_engine();

