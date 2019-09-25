void Cutscene_Action::init(TableNode* table) {
	this->which = tds_string2(table, WHICH_KEY);
}
bool Cutscene_Action::update(float dt) {
	game.do_cutscene(which);
	return true;
}
void Cutscene_Action::imgui_visualizer() {
	if (ImGui::TreeNode("Cutscene Action")) {
		string message = "Cutscene: " + which;
		ImGui::Text(message.c_str());
		ImGui::TreePop();
	}
}
