struct Points_Box;
struct Center_Box {
	glm::vec2 origin;
	glm::vec2 extents;
	
	static optional<Center_Box> from_entity(EntityHandle handle);
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

enum Collider_Kind : int {
	NO_COLLIDER,
	STATIC,
	//@simplify(joey :3) combine dynamic and hero
	DYNAMIC,
	HERO,
	DOOR,
	COUNT_COLLIDERS
};


bool collider_matrix[Collider_Kind::COUNT_COLLIDERS][Collider_Kind::COUNT_COLLIDERS];
void collider_matrix_add(Collider_Kind me, Collider_Kind other, bool should_test);
bool should_test_collision(Collider_Kind me, Collider_Kind other);
void init_collider_matrix();

