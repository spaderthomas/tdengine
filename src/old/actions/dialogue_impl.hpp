void Dialogue_Action::init(TableNode* table) {
	Dialogue_Tree* tree = new Dialogue_Tree;
	TableNode* dialogue_table = tds_table2(table, "dialogue");
	tree->init_from_table(dialogue_table);

	this->tree = tree;
	this->actor = actor;
}
bool Dialogue_Action::update(float dt) {
	Dialogue_Node* node = tree->traverse();
	if (active_layer->input.was_pressed(GLFW_KEY_1)) {
		node->set_response(0);
	}
	else if (active_layer->input.was_pressed(GLFW_KEY_2)) {
		node->set_response(1);
	}
	else if (active_layer->input.was_pressed(GLFW_KEY_3)) {
		node->set_response(2);
	}
	else if (active_layer->input.was_pressed(GLFW_KEY_4)) {
		node->set_response(3);
	}
	
	node->do_current_text();
	
	if (active_layer->input.was_pressed(GLFW_KEY_SPACE)) {
		// If the text we submitted has fully shown
		if (text_box.is_all_text_displayed()) {
			// If this node has no more text to show
			if (node->is_done()) {
				// Break if node is terminal
				if (node->terminal) {
					text_box.reset_and_hide();
					return true;
				}
				// Show responses if it is not
				else {
					std::string all_response_text;
					for (auto& response : node->responses) {
						all_response_text += response + "\r";
					}
					text_box.begin(all_response_text);
				}
			} else {
				node->should_submit_next_text = true;
			}
		}
		// If the set has shown fully (but not ALL dialogue), go to the next set
		else if (text_box.is_current_set_displayed()) {
			text_box.resume();
		}
		// If the dialogue has partially shown, skip to the end of the line set
		else {
			Line_Set& set = text_box.current_set();
			set.point = set.max_point;
		}
	}
	
	text_box.update(dt);
	return false;
}
void Dialogue_Action::imgui_visualizer() {
	if (ImGui::TreeNode("Dialogue Action")) {
		ImGui::TreePop();
	}
}
