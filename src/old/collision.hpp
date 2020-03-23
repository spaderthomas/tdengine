

struct Collision_Element {
	EntityHandle me;
	EntityHandle other;
};

struct Physics_System {
	std::vector<Collision_Element> collisions;
	std::vector<EntityHandle> movers;
	
	void debug_draw_bounding_box(EntityHandle handle, glm::vec4 color);
	void process(float dt);
};
Physics_System physics_system;


