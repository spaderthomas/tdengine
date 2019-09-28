string Collision_Component::name() { return "Collision_Component"; }
void Collision_Component::init_from_table(TableNode* table) {
	kind = (Collider_Kind)tds_int2(table, "kind");
	bounding_box.screen_center.x = tds_float2(table, "bounding_box", "center", "x"); 
	bounding_box.screen_center.y = tds_float2(table, "bounding_box", "center", "y");
	bounding_box.screen_extents.x = tds_float2(table, "bounding_box", "extents", "x");
	bounding_box.screen_extents.y = tds_float2(table, "bounding_box", "extents", "y");
}
void Collision_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Collision Component")) {
		ImGui::TreePop();
	}
}
