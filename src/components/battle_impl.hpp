std::string Battle_Component::name() { return "Battle_Component"; }

TableNode* Battle_Component::make_template() const {
	TableNode* self = new TableNode;

	tds_set2(self, health, HEALTH_KEY);

	TableNode* moves_table = new TableNode;
	tds_set2(self, moves_table, MOVES_KEY);

	fox_for(i, moves.size()) {
		tds_set2(moves_table, moves[i]->name, std::to_string(i));
	}

	return self;
}

void Battle_Component::init(TableNode* table) {
	this->health = tds_int2(table, "health");

	// Put the pointer to the move data that we parsed in at init time
	// into the component's list of moves
	TableNode* moves_table = tds_table2(table, "moves");
	for (int idx = 0; idx < moves_table->assignments.size(); idx++) {
		std::string move_name = tds_string2(moves_table, std::to_string(idx));
		moves.push_back(move_data[move_name]);
	}
}

void Battle_Component::imgui_visualizer() {
	static char move_name[256] = {0}; // @buffer
	static bool show_move_not_exist = false;
	if (ImGui::TreeNode("Battle Component")) {
		ImGui::SliderInt("Health", (int*)(&health), 0, 10);
		
		ImGui::InputText("##move_input", move_name, 256);
		ImGui::SameLine();
		if (ImGui::Button("Add Move!")) {
			auto move = move_data[move_name];
			if (!move) {
				show_move_not_exist = true;
			}
			else {
				moves.push_back(move);
				memset(move_name, 0, 256);
				show_move_not_exist = false;
			}
		}

		if (show_move_not_exist) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor_Red);
			ImGui::Text("That move doesn't exist.");
			ImGui::PopStyleColor();
		}
		ImGui::TreePop();
	}
}
