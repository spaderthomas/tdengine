void Movement_Component::init_from_table(TableNode* table) {
	speed.x = tds_float2(table, "speed", "x");
	speed.y = tds_float2(table, "speed", "y");
}

string Movement_Component::name(){
	return "Movement_Component";
}

void Movement_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Movement Component")) {
		ImGui::TreePop();
	}
}