enum Collider_Kind : int;
struct Collision_Component : Component {
	struct {
		glm::vec2 screen_center;
		glm::vec2 screen_extents;
	} bounding_box;
	Collider_Kind kind;
	
	void init_from_table(TableNode* table);
	string name() override;
	void imgui_visualizer() override;
};
