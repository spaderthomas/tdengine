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
		static vector<string> tile_size_descriptions = {
			"-1.5 tiles",
			"-1.25 tiles",
			"-1 tiles",
			"-.5 tiles",
			"0 tiles",
			".5 tiles",
			"1 tile",
			"1.25 tiles",
			"1.5 tiles",
			"2 tiles",
			"3 tiles",
			"4 tiles",
			"5 tiles",
			"6 tiles",
		};
		static vector<double> tile_sizes = {
			-0.0666,
			-0.0555,
			-0.0444,
			-0.0222,
			 0, 
			 0.0222,
			 0.0444,
			 0.0555,
			 0.0666,
			 0.0888,
			 0.1333,
     		 0.1777,
			 0.2222,
			 0.2666,
		};

		// @spader use a lambda you goon
		static int current_center_x = 4;
		if (ImGui::BeginCombo("Bounding Box Center X", tile_size_descriptions[current_center_x].c_str(), 0)) {
			fox_for(i, tile_size_descriptions.size()) {
				bool is_selected = current_center_x == i;
				if (ImGui::Selectable(tile_size_descriptions[i].c_str(), is_selected)) {
					current_center_x = i;
				}
			}
			
			ImGui::EndCombo();
		}
		bounding_box.screen_center.x = tile_sizes[current_center_x];

		static int current_center_y = 4;
		if (ImGui::BeginCombo("Bounding Box Center Y", tile_size_descriptions[current_center_y].c_str(), 0)) {
			fox_for(i, tile_size_descriptions.size()) {
				bool is_selected = current_center_y == i;
				if (ImGui::Selectable(tile_size_descriptions[i].c_str(), is_selected)) {
					current_center_y = i;
				}
			}
			
			ImGui::EndCombo();
		}
		bounding_box.screen_center.y = tile_sizes[current_center_y];

		static int current_extent_x = 5;
		if (ImGui::BeginCombo("Bounding Box Extents X", tile_size_descriptions[current_extent_x].c_str(), 0)) {
			fox_for(i, tile_size_descriptions.size()) {
				bool is_selected = current_extent_x == i;
				if (ImGui::Selectable(tile_size_descriptions[i].c_str(), is_selected)) {
					current_extent_x = i;
				}
			}
			
			ImGui::EndCombo();
		}
		bounding_box.screen_extents.y = tile_sizes[current_extent_x];

		static int current_extent_y = 5;
		if (ImGui::BeginCombo("Bounding Box Extents Y", tile_size_descriptions[current_extent_y].c_str(), 0)) {
			fox_for(i, tile_size_descriptions.size()) {
				bool is_selected = current_extent_y == i;
				if (ImGui::Selectable(tile_size_descriptions[i].c_str(), is_selected)) {
					current_extent_y = i;
				}
			}
			
			ImGui::EndCombo();
		}
		bounding_box.screen_extents.y = tile_sizes[current_extent_y];

		static vector<string> collider_kind_descriptions = {
			"No Collider",
			"Static",
			"Dynamic",
			"Hero",
			"Door"
		};
		
		static int current_collider_kind = 0;
		if (ImGui::BeginCombo("Collider Kind", collider_kind_descriptions[current_collider_kind].c_str(), 0)) {
			fox_for(i, collider_kind_descriptions.size()) {
				bool is_selected = current_collider_kind == i;
				if (ImGui::Selectable(collider_kind_descriptions[i].c_str(), is_selected)) {
					current_collider_kind = i;
				}
			}
			
			ImGui::EndCombo();
		}
		kind = (Collider_Kind)current_collider_kind;


		ImGui::TreePop();
	}
}
