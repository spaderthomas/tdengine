string Battle_Component::name() { return "Battle_Component"; }
void Battle_Component::init_from_table(TableNode* table) {
	this->health = tds_int2(table, "health");

	// Put the pointer to the move data that we parsed in at init time
	// into the component's list of moves
	TableNode* moves_table = tds_table2(table, "moves");
	for (int idx = 0; idx < moves_table->assignments.size(); idx++) {
		string move_name = tds_string2(moves_table, to_string(idx));
		moves.push_back(move_data[move_name]);
	}
}
void Battle_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Battle Component")) {
		ImGui::SliderInt("Health", (int*)(&health), 0, 10);
		ImGui::TreePop();
	}
}
