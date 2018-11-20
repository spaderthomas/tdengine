struct Collision_Element {
	EntityHandle me;
	EntityHandle other;
};

struct Physics_System {
	vector<Collision_Element> collisions;
	vector<EntityHandle> movers;

	void debug_draw_bounding_box(EntityHandle handle, glm::vec4 color);
	void process(float dt);
};
Physics_System physics_system;

bool are_boxes_colliding(Center_Box a, Center_Box b, glm::vec2& penetration);
bool are_boxes_colliding(Points_Box a, Points_Box b, glm::vec2& penetration);

enum Collider_Kind {
	STATIC,
	DYNAMIC,
	HERO,
	DOOR,
	NO_COLLIDER
};


bool collider_matrix[Collider_Kind::NO_COLLIDER][Collider_Kind::NO_COLLIDER] = {};
void collider_matrix_add(Collider_Kind me, Collider_Kind other, bool should_test);
bool should_test_collision(Collider_Kind me, Collider_Kind other);
void init_collider_matrix();

