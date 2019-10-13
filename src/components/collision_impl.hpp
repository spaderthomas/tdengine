string Collision_Component::name() { return "Collision_Component"; }

TableNode* Collision_Component::make_template() const {
	TableNode* self = new TableNode;

	TableNode* bounding_box_table = new TableNode;
	tds_set2(self, bounding_box_table, BOUNDING_BOX_KEY);
	
	TableNode* center_table = new TableNode;
	tds_set2(self, center_table, BOUNDING_BOX_KEY, CENTER_KEY);
	
	TableNode* extents_table = new TableNode;
	tds_set2(self, extents_table, BOUNDING_BOX_KEY, EXTENTS_KEY);

	tds_set2(self, bounding_box.screen_center.x, BOUNDING_BOX_KEY, CENTER_KEY, X_KEY);
	tds_set2(self, bounding_box.screen_center.y, BOUNDING_BOX_KEY, CENTER_KEY, X_KEY);
	tds_set2(self, bounding_box.screen_extents.x, BOUNDING_BOX_KEY, EXTENTS_KEY, X_KEY);
	tds_set2(self, bounding_box.screen_extents.y, BOUNDING_BOX_KEY, EXTENTS_KEY, X_KEY);

	tds_set2(self, (int)kind, KIND_KEY);

	return self;
}


void Collision_Component::init(TableNode* table) {
	kind = (Collider_Kind)tds_int2(table, "kind");
	bounding_box.screen_center.x = tds_float2(table, "bounding_box", "center", "x"); 
	bounding_box.screen_center.y = tds_float2(table, "bounding_box", "center", "y");
	bounding_box.screen_extents.x = tds_float2(table, "bounding_box", "extents", "x");
	bounding_box.screen_extents.y = tds_float2(table, "bounding_box", "extents", "y");
}
void Collision_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Collision Component")) {

		static auto imgui_combo_wrapper = [](const char* label, int& selected, vector<string>& choices) {
			if (ImGui::BeginCombo(label, choices[selected].c_str(), 0)) {
				fox_for(i, choices.size()) {
					bool is_selected = selected == i;
					if (ImGui::Selectable(choices[i].c_str(), is_selected)) {
						selected = i;
					}
				}
			
				ImGui::EndCombo();
			}
		};
		
		static int current_center_x = 4;
		imgui_combo_wrapper("Bounding Box Center X", current_center_x, tile_size_descriptions);
		bounding_box.screen_center.x = tile_sizes[current_center_x];

		static int current_center_y = 4;
		imgui_combo_wrapper("Bounding Box Center Y", current_center_y, tile_size_descriptions);
		bounding_box.screen_center.y = tile_sizes[current_center_y];

		static int current_extent_x = 5;
		imgui_combo_wrapper("Bounding Box Extents X", current_extent_x, tile_size_descriptions);
		bounding_box.screen_extents.y = tile_sizes[current_extent_x];

		static int current_extent_y = 5;
		imgui_combo_wrapper("Bounding Box Extents Y", current_extent_y, tile_size_descriptions);
		bounding_box.screen_extents.y = tile_sizes[current_extent_y];

		
		static int current_collider_kind = 0;
		imgui_combo_wrapper("Collider Kind", current_collider_kind, collider_kind_descriptions);
		kind = (Collider_Kind)current_collider_kind;

		ImGui::TreePop();
	}
}
