void Wait_For_Interaction_Action::init(TableNode* table) {}
bool Wait_For_Interaction_Action::update(float dt) {
	def_get_cmp(ic, actor.deref(), Interaction_Component);
	if (ic->was_interacted_with) {
		// Clear the IC so we don't call this again next frame.
		ic->was_interacted_with = false;
		ic->other = { 0, nullptr };
		
		return true;
	}
	
	return false;
}
void Wait_For_Interaction_Action::imgui_visualizer() {
	if (ImGui::TreeNode("Wait For Interaction Action")) {
		ImGui::TreePop();
	}
}
void Wait_For_Interaction_Action::set_actor(EntityHandle actor) {
	this->actor = actor;
};
