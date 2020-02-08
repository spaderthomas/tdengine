enum Collider_Kind : int;
struct Collision_Component : Component {
	struct {
		glm::vec2 screen_center;
		glm::vec2 screen_extents;
	} bounding_box;
	Collider_Kind kind;

	TableNode* make_template() const override;
	void init(TableNode* table) override;
	std::string name() override;
	void imgui_visualizer() override;
};
