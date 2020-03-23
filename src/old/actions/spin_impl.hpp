void Spin_Action::init(TableNode* table) {
}
bool Spin_Action::update(float dt) {
	return false;
}
void Spin_Action::imgui_visualizer() {
	if (ImGui::TreeNode("Spin Action")) {
		ImGui::TreePop();
	}
}
