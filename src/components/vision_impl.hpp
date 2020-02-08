void Vision_Component::init(TableNode* table) {
	width = tds_float2(table, "extents", "width");
	depth = tds_float2(table, "extents", "depth");
}

TableNode* Vision_Component::make_template() const {
	TableNode* self = new TableNode;
	
	TableNode* extents_table = new TableNode;
	tds_set2(self, extents_table, EXTENTS_KEY);
	
	tds_set2(self, width, EXTENTS_KEY, WIDTH_KEY);
	tds_set2(self, depth, EXTENTS_KEY, HEIGHT_KEY);

	return self;
}

std::string Vision_Component::name() {
	return "Vision_Component";
}

void Vision_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Vision Component")) {
		ImGui::TreePop();
	}
}
