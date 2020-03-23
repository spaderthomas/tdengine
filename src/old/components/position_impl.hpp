TableNode* Position_Component::save() const {
	TableNode* self = new TableNode;
	
	// Could make something to let you create nested tables if they don't exist,
	// but typo errors + explicit > implicit means I'll leave it for now
	TableNode* pos_table = new TableNode;
	tds_set2(self, pos_table, POS_KEY);
	
	tds_set2(self, world_pos.x, POS_KEY, "x");
	tds_set2(self, world_pos.y, POS_KEY, "y");
	
	return self;
}
void Position_Component::load(TableNode* self) {
	world_pos.x = tds_float2(self, POS_KEY, "x");
	world_pos.y = tds_float2(self, POS_KEY, "y");
}
std::string Position_Component::name() { return "Position_Component"; }
void Position_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Position Component")) {
		ImGui::SliderFloat2("Position", glm::value_ptr(world_pos), 0.f, 2.f);
		ImGui::TreePop();
	}
}
