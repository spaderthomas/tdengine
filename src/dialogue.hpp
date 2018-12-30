struct Dialogue_Node {
	string full_text;
	vector<string> responses;
	vector<Dialogue_Node*> children;
	int response = -1;

	bool already_drew_line = false; // To prevent us from spamming the text box with the same string
	bool terminal = false; // Terminal nodes exit dialogue on spacebar

	void set_response(int response);
	bool has_response();
	Dialogue_Node* next();
	void show_line();
	void init_from_table(sol::table& table);
	void init_from_table(TableNode* table);
};
struct Dialogue_Tree {
	Dialogue_Node* root;
	string npc;
	string scene;

	Dialogue_Node* traverse();
	void save();
	void load();
	void init_from_table(sol::table table);
	void init_from_table(TableNode* table);
};