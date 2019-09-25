void Set_State_Action::init(TableNode* table) {
	this->var = tds_string2(table, "var");
	this->value = tds_bool2(table, "value");
}
bool Set_State_Action::update(float dt) {
	update_state(this->var, this->value);
	return true;
}
void Set_State_Action::imgui_visualizer() {
	if (ImGui::TreeNode("Set State Action")) {
		string which = "State: " + this->var;
		string val = this->value ? "Value: true" : "Value: false";
		ImGui::Text(which.c_str());
		ImGui::Text(val.c_str());
		ImGui::TreePop();
	}
}
