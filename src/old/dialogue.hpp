struct Dialogue_Node {
	std::vector<std::string> all_text;
	std::vector<std::string> responses;
	std::vector<Dialogue_Node*> children;
	int response = -1;
	int text_idx = 0;

	bool should_submit_next_text = false; // To prevent us from spamming the text box with the same string
	bool terminal = false; // Terminal nodes exit dialogue on spacebar

	void set_response(int response);
	bool has_response();
	Dialogue_Node* next();
	void do_current_text();
	bool is_done();
	void init_from_table(TableNode* table);
};
struct Dialogue_Tree {
	Dialogue_Node* root;
	std::string npc;
	std::string scene;

	Dialogue_Node* traverse();
	void save();
	void load();
	void init_from_table(TableNode* table);
};