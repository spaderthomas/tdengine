string Battle_Component::name() { return "Battle_Component"; }

TableNode* Battle_Component::make_template() const {
	TableNode* self = new TableNode;

	tds_set2(self, health, HEALTH_KEY);

	TableNode* moves_table = new TableNode;
	tds_set2(self, moves_table, MOVES_KEY);

	fox_for(i, moves.size()) {
		tds_set2(moves_table, to_string(1), moves[i]->name);
	}

	return self;
}

void Battle_Component::init(TableNode* table) {
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
