using Position = glm::vec2;

struct Collider {
	int entity;
	glm::vec2 extents;
	glm::vec2 offset;
};

struct CollisionInfo {
	int entity;
	int other;
};

namespace MoveFlags {
	constexpr int BypassCollision = 1 << 0;
	constexpr int AbsolutePosition = 1 << 1;
}
	
struct MoveRequest {
	int entity;
	glm::vec2 wish;
	int flags = 0;
};

struct AttachedPosition {
	int entity;
	int attached_to;
	glm::vec2 offset;
};

struct Points_Box;
struct Center_Box {
	glm::vec2 origin;
	glm::vec2 extents;
	
	static Center_Box from_points(Points_Box& points);
	static Center_Box from_entity(int entity);
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
bool point_inside_entity(float x, float y, int entity);

bool are_boxes_colliding(Center_Box a, Center_Box b, glm::vec2& penetration);
bool are_boxes_colliding(Points_Box a, Points_Box b, glm::vec2& penetration);
bool are_entities_colliding(int a, int b, glm::vec2& penetration);
Center_Box box_from_collider(Position position, Collider collider);

struct PhysicsEngine {
	std::map<int, Position> positions;
	std::map<int, Collider> collidable;
	std::map<int, Collider> raycast;
	std::map<int, Collider> triggers;

	std::vector<AttachedPosition> attached_entities;
	
	std::vector<MoveRequest> requests;

	// These are built up when the engine resolves all physics / collisions.
	// The update system reads from these, runs the appropriate callbacks,
	// and then this struct clears out the vectorvector on the next frame.
	std::vector<CollisionInfo> collisions;
	
	void      update(float dt);
	
	bool      has_collider(int entity);
	
	Collider* get_collider(int entity);	
	Collider* get_raycast(int entity);
	Collider* get_trigger(int entity);
	Position* get_position(int entity);
	
	void      add_position(int entity, Position position);
	void      add_collider(int entity, Collider collider);
	void      add_raycast(int entity, Collider collider);
	void      add_trigger(int entity, Collider collider);
	
	void      remove_entity(int entity);
	int       ray_cast(float x, float y);

	void      push_position_to_lua(int entity);
	void      detach_position(int entity);
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
	std::map<int, Interactable> interactables;
	
	int interacted_with = -1;
	Collider player_vision;
	
	// Game code can flip this flag e.g. when the player presses a key to
	// interact with stuff. Otherwise, we won't check for interactions.
	bool check_for_interactions = false;

	void          update(float dt);
	void          remove_entity(int entity);
	void          add_interactable(int entity, Interactable interactable);
	Interactable* get_interactable(int entity);
};
InteractionSystem& get_interaction_system();
