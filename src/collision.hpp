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


// Lua exports
bool are_entities_colliding(EntityHandle a, EntityHandle b);
void register_potential_collision(EntityHandle me, EntityHandle other);
