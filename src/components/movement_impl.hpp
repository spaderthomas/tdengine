void Movement_Component::init(TableNode* table) {
	speed.x = tds_float2(table, "speed", "x");
	speed.y = tds_float2(table, "speed", "y");

	wish = glm::vec2(0.f);
}

std::string Movement_Component::name(){
	return "Movement_Component";
}

void Movement_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Movement Component")) {
		defer { ImGui::TreePop(); };
		
		ImGui::SliderFloat2("Speed", glm::value_ptr(this->speed), 0.f, .005f);
	}
}

TableNode* Movement_Component::make_template() const {
	TableNode* self = new TableNode;

	TableNode* speed_t = new TableNode;
	tds_set2(self, speed_t, SPEED_KEY);

	tds_set2(speed_t, this->speed.x, X_KEY);
	tds_set2(speed_t, this->speed.y, Y_KEY);

	return self;
}
