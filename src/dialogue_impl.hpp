 void Dialogue_Node::set_response(int response) {
	if (response >= (int)children.size()) return;
	this->response = response;
}
bool Dialogue_Node::has_response() {
	return response != -1;
}
Dialogue_Node* Dialogue_Node::next() {
	if (has_response()) return children[response];
	return nullptr;
}
void Dialogue_Node::do_current_text() {
	if (should_submit_next_text) {
		text_box.begin(all_text[text_idx]);
		text_idx++;
	}
	should_submit_next_text = false;
}
bool Dialogue_Node::is_done() {
	return text_idx == all_text.size();
}

void Dialogue_Node::init_from_table(TableNode* table) {
	all_text.clear();
	responses.clear();
	response = -1;
	children.clear();
	should_submit_next_text = true;

	terminal = tds_table2(table, "terminal");
	
	TableNode* texts = tds_table2(table, TEXT_KEY);
	fox_for(i, texts->assignments.size()) {
		this->all_text.push_back(tds_string2(texts, std::to_string(i)));
	}
	
	TableNode* responses = tds_table2(table, "responses");
	fox_for(resp_idx, responses->assignments.size()) {
		this->responses.push_back(tds_string2(responses, std::to_string(resp_idx)));
	}

	TableNode* children = tds_table2(table, "children");
	fox_for(child_idx, children->assignments.size()) {
		TableNode* child_table = tds_table2(children, std::to_string(child_idx));
		Dialogue_Node* child = new Dialogue_Node;
		child->init_from_table(child_table);
		this->children.push_back(child);
	}
}
Dialogue_Node* Dialogue_Tree::traverse() {
	Dialogue_Node* cur = root;
	while (cur && cur->has_response()) {
		cur = cur->children[cur->response];
	}

	return cur;
}

// Writes the dialogue tree into dialogue.json under the keys given by npc and scene
// Requires that you have an npc and a scene set
void Dialogue_Tree::save() {
	fox_assert(npc != "");
	fox_assert(scene != "");
	// Load in all of the saved dialogue
	std::string path = absolute_path("save/dialogue.json");
	std::ifstream save_file(path);
	json dialogue;
	save_file >> dialogue;

	// Make appropriate keys if they don't exist
	json this_scene_dialogue;
	if (dialogue[npc].is_null()) {
		// We don't have an entry for this NPC's saved dialogue at all
		json all_npc_dialogue = dialogue[npc] = json::object();
		all_npc_dialogue[scene] = json::array();
		this_scene_dialogue = all_npc_dialogue[scene];
	}
	else {
		// There is an entry for this NPC's saved dialogue; we overwrite it
		dialogue[npc][scene] = json::array();
		this_scene_dialogue = dialogue[npc][scene];
	}

	// Traverse the tree and mark decisions
	Dialogue_Node* cur = root;
	while (cur && cur->has_response()) {
		this_scene_dialogue.push_back(cur->response);
		cur = cur->next();
	}

	// Save it
	dialogue[npc][scene] = this_scene_dialogue;
	std::ofstream save_file_out(path);
	save_file_out << std::setw(4) << dialogue << std::endl;
}

void Dialogue_Tree::load() {
	fox_assert(npc != "");
	fox_assert(scene != "");

	// Load in the JSON from dialogue.json
	std::string path = absolute_path("save/dialogue.json");
	std::ifstream save_file(path);
	json dialogue;
	save_file >> dialogue;

	// Traverse the tree by the responses in JSON, marking them as you go
	json this_scene_dialogue = dialogue[npc][scene];
	Dialogue_Node* cur = root;
	fox_iter(it, this_scene_dialogue) {
		cur->response = *it;
		cur->should_submit_next_text = true; //@hack -- should probably figure out where to actually do this
		cur = cur->children[*it];
	}

}

void Dialogue_Tree::init_from_table(TableNode* table) {
	root = new Dialogue_Node;
	root->init_from_table(table);
}
