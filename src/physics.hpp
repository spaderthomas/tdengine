struct Collider {
	int entity;
	glm::vec2 origin;
	glm::vec2 extents;
};

namespace MoveFlags {
	constexpr int BypassCollision = 1;
}
	
struct MoveRequest {
	int entity;
	glm::vec2 wish;
	int flags = 0;
};

struct Points_Box;
struct Center_Box {
	glm::vec2 origin;
	glm::vec2 extents;
	
	static Center_Box from_points(Points_Box& points);
	Points_Box as_points();
};

struct Points_Box {
	float top;
	float bottom;
	float left;
	float right;
	
	void convert_screen_to_gl();
	Center_Box as_center_box();
};
bool point_inside_box(glm::vec2& screen_pos, Center_Box& box);

bool are_boxes_colliding(Center_Box a, Center_Box b, glm::vec2& penetration);
bool are_boxes_colliding(Points_Box a, Points_Box b, glm::vec2& penetration);

struct PhysicsEngine {
	std::map<EntityID, Collider> colliders;
	std::vector<MoveRequest> requests;
	
	void update(float dt);
	bool has_collider(int entity);
	void add_collider(int entity, Collider collider);
	Collider* get_collider(int entity);
};

PhysicsEngine& get_physics_engine();

