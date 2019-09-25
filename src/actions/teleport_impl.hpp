void Teleport_Action::init(TableNode* table) {
	this->actor = actor;
	this->x = tds_float2(table, "dest", "x");
	this->y = tds_float2(table, "dest", "y");
}
		
bool Teleport_Action::update(float dt) {
	teleport_entity(this->actor, this->x, this->y);
	return true;
}
void Teleport_Action::imgui_visualizer() {
	if (ImGui::TreeNode("Teleport Action")) {
		ImGui::TreePop();
	}
}
