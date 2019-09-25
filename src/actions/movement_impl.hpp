void Movement_Action::init(TableNode* table) {
	this->dest.x = tds_float2(table, "dest", "x"); 
	this->dest.y = tds_float2(table, "dest", "y");
	this->actor = actor;
}
bool Movement_Action::update(float dt) { 
	def_get_cmp(pc, actor.deref(), Position_Component);
	
	if (vec_almost_equals(pc->world_pos, dest)) {
		return true;
	}
	
	bool up = pc->world_pos.y < dest.y;
	bool down = pc->world_pos.y > dest.y;
	bool right = pc->world_pos.x < dest.x;
	bool left = pc->world_pos.x > dest.x;
	
	move_entity(actor, up, down, left, right);
	return false;
}	
void Movement_Action::imgui_visualizer() {
	if (ImGui::TreeNode("Movement Action")) {
		ImGui::Text("(%f, %f)", dest.x, dest.y);
		ImGui::TreePop();
	}
}

