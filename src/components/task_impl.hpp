string Task_Component::name() { return "Task_Component"; }
void Task_Component::init_from_table(TableNode* table) {
	this->task = new Task;
}
void Task_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Task Component")) {
		task->imgui_visualizer();
		ImGui::TreePop();
	}
}
