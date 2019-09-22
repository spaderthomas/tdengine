void Move::init_from_table(TableNode* table) {
	this->power = tds_int2(table, "power");
}

void init_moves() {
	TableNode* tds_move_data = tds_table("moves");
	for (auto& kvp : tds_move_data->assignments) {
		Move* move = new Move;
		move->name = kvp->key;
		TableNode* move_as_table = tds_table2(tds_move_data, kvp->key);
		move->init_from_table(move_as_table);

		move_data[move->name] = move;
	}
}
