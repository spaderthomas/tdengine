string Interaction_Component::name() {
	return "Interaction_Component";
}

void Interaction_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Interaction Component")) {
		ImGui::TreePop();
	}
}
