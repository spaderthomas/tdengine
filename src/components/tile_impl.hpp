string Tile_Component::name() { return "Tile_Component"; }
TableNode* Tile_Component::save() const {
	TableNode* self = new TableNode;

	tds_set2(self, x, "x");
	tds_set2(self, y, "y");

	return self;
}
void Tile_Component::load(TableNode* self) {
	this->x = tds_int2(self, "x");
	this->y = tds_int2(self, "y");
}
void Tile_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Tile Component")) {
		int min = x - 20;
		int max = x + 20;
		ImGui::SliderInt("Position", &x, min, max);
		ImGui::TreePop();
	}
}
