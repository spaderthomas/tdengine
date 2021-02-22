struct Collider {
	int entity;
	glm::vec2 origin;
	glm::vec2 extents;
	glm::vec2 offset;

	bool collision_detection_enabled = true;
};

struct CollisionInfo {
	int entity;
	int other;
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
	static Center_Box from_collider(Collider* collider);
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
bool point_inside_collider(float x, float y, const Collider& collider);

bool are_boxes_colliding(Center_Box a, Center_Box b, glm::vec2& penetration);
bool are_boxes_colliding(Points_Box a, Points_Box b, glm::vec2& penetration);
bool are_colliding(Collider a, Collider b, glm::vec2& penetration);

struct PhysicsEngine {
	std::map<int, Collider> colliders;
	std::vector<MoveRequest> requests;

	// These are built up when the engine resolves all physics / collisions.
	// The entity manager reads from this on the next frame and calls the relevant
	// callbacks in Lua. The engine will clear out all data from here before it
	// updates.
	std::vector<CollisionInfo> collisions;
	
	void update(float dt);
	bool has_collider(int entity);
	void add_collider(int entity, Collider collider);
	Collider* get_collider(int entity);
	void remove_entity(int entity);
};
PhysicsEngine& get_physics_engine();

// Shit that the player can interact with
struct Interactable {
	int entity;
	glm::vec2 extents;
	glm::vec2 offset;
};

// Shit that looks for the player and does something when it sees them
struct Watcher {
	int entity;
	glm::vec2 extents;
	glm::vec2 offset;
};

struct InteractionSystem {
	std::vector<Interactable> interactables;
	std::vector<Watcher> watchers;
	
	int player = -1;
	int interacted_with = -1;
	
	// Game code can flip this flag e.g. when the player presses a key to
	// interact with stuff. Otherwise, we won't check for interactions.
	bool check_for_interactions = false;

	void update(float dt);
};
InteractionSystem& get_interaction_system();
