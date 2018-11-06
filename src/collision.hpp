bool are_boxes_colliding(Center_Box a, Center_Box b, glm::vec2& penetration);
struct PhysicsSystem {
	vector<pool_handle<Entity>> entity_handles;

	void debug_draw_bounding_box(pool_handle<Entity> handle, glm::vec4 color);
	void process(float dt);
};
PhysicsSystem physics_system;


// Lua exports
bool are_entities_colliding(EntityHandle a, EntityHandle b);