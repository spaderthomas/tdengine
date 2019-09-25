void Camera_Follow_Action::init(TableNode* table) {
	this->who = tds_string2(table, "who");
	if (table->has_key(PAN_KEY)) this->pan = tds_bool2(table, "pan");
}
bool Camera_Follow_Action::update(float dt) {
	auto& camera = game.camera;
	auto entity = game.active_level->get_first_matching_entity(this->who);

	// If we don't pan, it's simple: Tell the camera to follow it, and we're done
	if (!pan) {
		camera.follow(entity);
		return true;
	}

	// Make the camera track the entity (once)
	if (is_first_update && pan) {
		camera.pan_and_follow(entity);
	}
	is_first_update = false;

	// Return when the camera is at the same position as the entity.
	return camera.is_at_entity(camera.following);
}
void Camera_Follow_Action::imgui_visualizer() {
	if (ImGui::TreeNode("Camera Follow Action")) {
		ImGui::TreePop();
	}
}
