struct Move {
	std::string name;
	int power;

	void init_from_table(TableNode* table);
};

std::unordered_map<std::string, Move*> move_data;
void init_moves();
