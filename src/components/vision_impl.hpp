void Vision_Component::init(TableNode* table) {
	width = tds_float2(table, "extents", "width");
	depth = tds_float2(table, "extents", "depth");
}

string Vision_Component::name() {
	return "Vision";
}

void Vision_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Vision Component")) {
		ImGui::TreePop();
	}
}
