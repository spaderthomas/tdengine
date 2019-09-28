void Door_Component::init_from_table(TableNode* table) {
	level = tds_string2(table, LEVEL_KEY);
}

TableNode* Door_Component::save() const {
	TableNode* self = new TableNode;
	tds_set2(self, level, LEVEL_KEY);
	
	return self;
}

void Door_Component::load(TableNode* self) {
	level = tds_string2(self, LEVEL_KEY);
}

string Door_Component::name() {
	return "Door_Component";
}

void Door_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Door Component")) {
		if (ImGui::BeginCombo("##setdoor", level.c_str(), 0)) {
			for (auto& kvp : levels) {
				const string& name = kvp.first;
				bool is_selected = level.c_str() == name;
				if (ImGui::Selectable(name.c_str(), &is_selected)) {
					level = name;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::TreePop();
	}
}
