void Camera_Pan_Action::init(TableNode* table) {
	this->dest.x = tds_float2(table, "dest", "x");
	this->dest.y = tds_float2(table, "dest", "y");
	this->count_frames = tds_int2(table, "frames");
}
bool Camera_Pan_Action::update(float dt) {
	auto layer = all_layers[iactive_layer];
	layer->camera.offset += dest / (float)count_frames;
	
	frames_elapsed++;
	return count_frames == frames_elapsed;
}
void Camera_Pan_Action::imgui_visualizer() {
	if (ImGui::TreeNode("Camera Pan Action")) {
		ImGui::TreePop();
	}
}
