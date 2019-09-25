void And_Action::init(TableNode* table) {
	TableNode* actions = tds_table2(table, "actions");
	fox_for(action_idx, actions->assignments.size()) {
		TableNode* action_table = tds_table2(table, "actions", to_string(action_idx));
		this->actions.push_back(action_from_table(action_table, actor));
	}
}

bool And_Action::update(float dt) {
	bool done = true;
	for (auto it = actions.begin(); it != actions.end();) {
		Action* action = *it;
		if (action->update(dt)) {
			it = actions.erase(it);
		}
		else {
			done = false;
			it++;
		}
	}

	return done;
}
void And_Action::imgui_visualizer() {
	if (ImGui::TreeNode("And Action")) {
		ImGui::TreePop();
	}
}
