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
void Dialogue_Node::show_line() {
	if (!already_drew_line)
		text_box.begin(full_text);
	already_drew_line = true;
}

void Dialogue_Node::init_from_table(sol::table& table) {
	//@leak
	full_text.clear();
	responses.clear();
	response = -1;
	children.clear();
	already_drew_line = false;

	terminal = table["terminal"];
	full_text = table["text"];
	sol::table responses = table["responses"];
	for (auto& pair : responses) {
		this->responses.push_back(pair.second.as<string>());
	}

	sol::table children = table["children"];
	for (auto& pair : children) {
		int index = pair.first.as<int>();
		sol::table child = pair.second;
		Dialogue_Node* child_node = new Dialogue_Node;
		child_node->init_from_table(child);
		this->children.push_back(child_node);
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
	string path = absolute_path("save\\dialogue.json");
	ifstream save_file(path);
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
	ofstream save_file_out(path);
	save_file_out << std::setw(4) << dialogue << std::endl;
}

void Dialogue_Tree::load() {
	fox_assert(npc != "");
	fox_assert(scene != "");

	// Load in the JSON from dialogue.json
	string path = absolute_path("save\\dialogue.json");
	ifstream save_file(path);
	json dialogue;
	save_file >> dialogue;

	// Traverse the tree by the responses in JSON, marking them as you go
	json this_scene_dialogue = dialogue[npc][scene];
	Dialogue_Node* cur = root;
	fox_iter(it, this_scene_dialogue) {
		cur->response = *it;
		cur->already_drew_line = false; //@hack -- should probably figure out where to actually do this
		cur = cur->children[*it];
	}

}

void Dialogue_Tree::init_from_table(sol::table table) {
	root = new Dialogue_Node;
	root->init_from_table(table);
}