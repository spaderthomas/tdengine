struct Move {
	string name;
	int power;

	void init_from_table(TableNode* table);
};

unordered_map<string, Move*> move_data;
void init_moves();
