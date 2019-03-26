// Surely there is a better way to not re-write all this code, but, hey, it works
void TableNode::set(vector<string> keys, int value) {
	// Get the table that we want to write to
	string value_key = keys.back();
	keys.erase(keys.end() - 1);
	TableNode* containing_table = get_table(keys);
	fox_assert(containing_table);
	
	// Search through existing KVPs to see if we're setting an existing one
	bool found = false;
	for (auto& assignment : containing_table->assignments) {
		KVPNode* kvp = (KVPNode*)assignment;
		if (kvp->key == value_key) {
			// Make sure it's the right type, then set it
			fox_assert(kvp->value->type == ASTNodeType::ANK_IntegerNode);
			IntegerNode* as_int_node = (IntegerNode*)kvp->value;
			as_int_node->value = value;
			
			found = true;
			break;
		}
	}
	
	// If we didn't find it, make a new node
	if (!found) {
		IntegerNode* new_value_node = new IntegerNode;
		new_value_node->value = value;
		
		KVPNode* new_node = new KVPNode;
		new_node->key = value_key;
		new_node->value = new_value_node;
		
		containing_table->assignments.push_back(new_node);
	}
}
void TableNode::set(vector<string> keys, string value) {
	// Get the table that we want to write to
	string value_key = keys.back();
	keys.erase(keys.end() - 1);
	TableNode* containing_table = get_table(keys);
	fox_assert(containing_table);
	
	// Search through existing KVPs to see if we're setting an existing one
	bool found = false;
	for (auto& assignment : containing_table->assignments) {
		KVPNode* kvp = (KVPNode*)assignment;
		if (kvp->key == value_key) {
			// Make sure it's the right type, then set it
			fox_assert(kvp->value->type == ASTNodeType::ANK_StringLiteralNode);
			StringLiteralNode* as_sl_node = (StringLiteralNode*)kvp->value;
			as_sl_node->value = value;
			
			found = true;
			break;
		}
	}
	
	// If we didn't find it, make a new node
	if (!found) {
		StringLiteralNode* new_value_node = new StringLiteralNode;
		new_value_node->value = value;
		
		KVPNode* new_node = new KVPNode;
		new_node->key = value_key;
		new_node->value = new_value_node;
		
		containing_table->assignments.push_back(new_node);
	}
}
void TableNode::set(vector<string> keys, float value) {
	// Get the table that we want to write to
	string value_key = keys.back();
	keys.erase(keys.end() - 1);
	TableNode* containing_table = get_table(keys);
	fox_assert(containing_table);
	
	// Search through existing KVPs to see if we're setting an existing one
	bool found = false;
	for (auto& assignment : containing_table->assignments) {
		KVPNode* kvp = (KVPNode*)assignment;
		if (kvp->key == value_key) {
			// Make sure it's the right type, then set it
			fox_assert(kvp->value->type == ASTNodeType::ANK_FloatNode);
			FloatNode* as_float_node = (FloatNode*)kvp->value;
			as_float_node->value = value;
			
			found = true;
			break;
		}
	}
	
	// If we didn't find it, make a new node
	if (!found) {
		FloatNode* new_value_node = new FloatNode;
		new_value_node->value = value;
		
		KVPNode* new_node = new KVPNode;
		new_node->key = value_key;
		new_node->value = new_value_node;
		
		containing_table->assignments.push_back(new_node);
	}
}
void TableNode::set(vector<string> keys, bool value) {
	// Get the table that we want to write to
	string value_key = keys.back();
	keys.erase(keys.end() - 1);
	TableNode* containing_table = get_table(keys);
	fox_assert(containing_table);
	
	// Search through existing KVPs to see if we're setting an existing one
	bool found = false;
	for (auto& assignment : containing_table->assignments) {
		KVPNode* kvp = (KVPNode*)assignment;
		if (kvp->key == value_key) {
			// Make sure it's the right type, then set it
			fox_assert(kvp->value->type == ASTNodeType::ANK_BoolNode);
			BoolNode* as_bool_node = (BoolNode*)kvp->value;
			as_bool_node->value = value;
			
			found = true;
			break;
		}
	}
	
	// If we didn't find it, make a new node
	if (!found) {
		BoolNode* new_value_node = new BoolNode;
		new_value_node->value = value;
		
		KVPNode* new_node = new KVPNode;
		new_node->key = value_key;
		new_node->value = new_value_node;
		
		containing_table->assignments.push_back(new_node);
	}
}
void TableNode::set(vector<string> keys, TableNode* value) {
	// Get the table that we want to write to
	string value_key = keys.back();
	keys.erase(keys.end() - 1);
	TableNode* containing_table = get_table(keys);
	fox_assert(containing_table);
	
	// Search through existing KVPs to see if we're setting an existing one
	bool found = false;
	for (auto& assignment : containing_table->assignments) {
		KVPNode* kvp = (KVPNode*)assignment;
		if (kvp->key == value_key) {
			// Make sure it's the right type, then set it
			fox_assert(kvp->value->type == ASTNodeType::ANK_TableNode);
			kvp->value = value;
			
			found = true;
			break;
		}
	}
	
	// If we didn't find it, make a new node
	if (!found) {
		KVPNode* new_node = new KVPNode;
		new_node->key = value_key;
		new_node->value = value;
		
		containing_table->assignments.push_back(new_node);
	}
}

template<typename T>      
void TableNode::push_back(T value) {
	vector<string> keys = { to_string(assignments.size()) };
	this->set(keys, value);
}

template<typename T>
Primitive TableNode::get(vector<string>& keys) {
	string value_key = keys.back();
	keys.erase(keys.end() - 1);
	TableNode* containing_table = get_table(keys);
	fox_assert(containing_table);
	
	for (auto& node : containing_table->assignments) {
		KVPNode* assignment = (KVPNode*)node;
		if (assignment->key != value_key) continue;
		
		ASTNode* payload = assignment->value;
		Primitive prim;
		if (typeid(T) == typeid(int)) {
			fox_assert(payload->type == ASTNodeType::ANK_IntegerNode);
			prim.type = Primitive::Type::INTEGER;
			prim.values.intval = ((IntegerNode*)payload)->value;
		}
		else if (typeid(T) == typeid(string)) {
			fox_assert(payload->type == ASTNodeType::ANK_StringLiteralNode);
			prim.type = Primitive::Type::STRING;
			prim.values.strval = ((StringLiteralNode*)payload)->value.c_str();
		}
		else if (typeid(T) == typeid(float)) {
			prim.type = Primitive::Type::FLOAT;
			fox_assert(payload->type == ASTNodeType::ANK_FloatNode || payload->type == ASTNodeType::ANK_IntegerNode);
			
			if (payload->type == ASTNodeType::ANK_FloatNode) {
				prim.values.floatval = ((FloatNode*)payload)->value;
			}
			else if (payload->type == ASTNodeType::ANK_IntegerNode) {
				prim.values.floatval = (float)((IntegerNode*)payload)->value;
			}
		}
		else if (typeid(T) == typeid(bool)) {
			fox_assert(payload->type == ASTNodeType::ANK_BoolNode);
			prim.type = Primitive::Type::BOOL;
			prim.values.boolval = ((BoolNode*)payload)->value;
		}
		return prim;
	}
	
	
	Primitive error;
	error.type = Primitive::Type::PRIM_ERROR;
	return error;
}
int TableNode::get_int(vector<string> keys) {
	Primitive prim = get<int>(keys);
	fox_assert(prim.type == Primitive::Type::INTEGER);
	return prim.values.intval;
}
string TableNode::get_string(vector<string> keys) {
	Primitive prim = get<string>(keys);
	fox_assert(prim.type == Primitive::Type::STRING);
	return prim.values.strval;
}
float TableNode::get_float(vector<string> keys) {
	Primitive prim = get<float>(keys);
	fox_assert(prim.type == Primitive::Type::FLOAT);
	return prim.values.floatval;
}
bool TableNode::get_bool(vector<string> keys) {
	Primitive prim = get<bool>(keys);
	fox_assert(prim.type == Primitive::Type::BOOL);
	return prim.values.boolval;
}
TableNode* TableNode::get_table(vector<string> keys) {
    if (!keys.size()) return this;
	
	TableNode* current_scope = this;
	string error_msg = "KeyError: global_scope"; // Write this here so that we know exactly what key we failed at
	for (int key_idx = 0; key_idx < (int)keys.size(); key_idx++) {
		auto& key = keys[key_idx];
		
		bool found = false;
		for (ASTNode* node : current_scope->assignments) {
			fox_assert(node->type == ASTNodeType::ANK_AssignNode);
			
			KVPNode* assignment = (KVPNode*)node;
			if (assignment->key == key) {
				found = true;
				
				// Move on to the next table if there are keys left
				ASTNode* next_scope = assignment->value;
				fox_assert(next_scope->type = ASTNodeType::ANK_TableNode);
				current_scope = (TableNode*)next_scope;
				
				// Return the table if we're at the end
				if (key_idx == keys.size() - 1) return current_scope;
				
				// Write the table we just indexed into to the erorr message
				error_msg += "[" + key + "]";
				break;
			}
		}
		
		if (!found) {
			// If we go through all assignments in a table without finding the next one, that's a key error
			error_msg += "[" + key + "]";
			tdns_log.write(error_msg);
		}
	}
	
	return nullptr;
}
ASTNode* TableNode::get_raw(vector<string> keys) {
	if (!keys.size()) return this;
	
	TableNode* current_scope = this;
	string error_msg = "KeyError: global_scope"; // Write this here so that we know exactly what key we failed at
	for (int key_idx = 0; key_idx < (int)keys.size(); key_idx++) {
		auto& key = keys[key_idx];
		
		bool found = false;
		for (ASTNode* node : current_scope->assignments) {
			fox_assert(node->type == ASTNodeType::ANK_AssignNode);
			
			KVPNode* assignment = (KVPNode*)node;
			if (assignment->key == key) {
				found = true;
				
				if (key_idx == keys.size() - 1) {
					return assignment->value;
				}
				
				// Move on to the next table if there are keys left
				ASTNode* next_scope = assignment->value;
				fox_assert(next_scope->type = ASTNodeType::ANK_TableNode);
				current_scope = (TableNode*)next_scope;
				
				// Write the table we just indexed into to the erorr message
				error_msg += "[" + key + "]";
				break;
			}
		}
		
		if (!found) {
			// If we go through all assignments in a table without finding the next one, that's a key error
			error_msg += "[" + key + "]";
			tdns_log.write(error_msg);
		}
	}
	
	return nullptr;
}
ASTNode* TableNode::maybe_key(string key) {
	for (auto node : assignments) {
		KVPNode* kvp = (KVPNode*)node;
		if (kvp->key == key) return kvp;
	}
	
	return nullptr;
}
void TableNode::dump(TableWriter& output) {
	output.begin_table();
	for (auto& assignment : assignments) {
		assignment->dump(output);
	}
	output.end_table();
}

void KVPNode::dump(TableWriter& output) {
	// Figure out if the key is numeric (i.e. we're in an array)
	bool is_numeral = true;
	for (auto& c : key) {
		if (c < '0' || c > '9') {
			is_numeral = false;
			break;
		}
	}
	
	if (!is_numeral) {
		string defn = key + " = ";
		output.write_line(defn);
		output.same_line();
	}
	
	value->dump(output);
}

void IntegerNode::dump(TableWriter& output) {
	output.write_line(to_string(value));
}
void FloatNode::dump(TableWriter& output) {
	output.write_line(to_string(value));
}
void BoolNode::dump(TableWriter& output) {
	output.write_line(to_string(value));
}
void StringLiteralNode::dump(TableWriter& output) {
	string as_literal = "\"" + value + "\"";
	output.write_line(as_literal);
}

bool Lexer::init(string script_path) {
	file = ifstream(script_path);
	fox_assert(file.good());
	file.seekg(0, ios_base::end);
	auto file_size = file.tellg();
	file.seekg(0, ios_base::beg);
	
	tokens.clear();
	line_number = 1;
	token_idx = 0;
	
	if (!file_size) {
		tdns_log.write(string("Script_path ") + script_path + " is empty.");
		return false;
	}
	
	return true;
}

void Lexer::lex() {
	token_idx = 0;
	while (true) {
		Token current_token = next_token_internal();
		tokens.push_back(current_token);
		if (current_token.type == Token::Type::_EOF) {
			return;
		}
	}
}

Token Lexer::next_token_internal() {
	while (is_whitespace(file.peek())) {
		if (file.peek() == '\n') line_number++;
		file.get();
	}
	
	char c;
	
	while (true) {
		if (file.eof()) {
			Token token;
			token.type = Token::Type::_EOF;
			return token;
		}
		
		file.get(c);
		
		// Scan for identifiers and bools
		if (is_alpha(c)) {
			Token token;
			token.type = Token::Type::IDENTIFIER;
			token.str_val = c;
			
			while (is_valid_identifier_char(file.peek())) {
				token.str_val.push_back(file.get());
			}
			
			if (token.str_val == "true" || token.str_val == "false") {
				token.type = Token::Type::BOOL;
				token.bool_val = token.str_val == "true" ? true : false;
				token.str_val = "";
			}
			
			return token;
		}
		// Scan for string literals
		else if (c == '"') {
			Token token;
			token.type = Token::Type::STRING_LITERAL;
			
			while (file.peek() != '"') {
				token.str_val.push_back(file.get());
			}
			file.get(); // Eat the final quotation mark
			
			return token;
		}
		// Scan for numbers
		else if (is_valid_number_start(c)) {
			Token token;
			
			bool is_negative = false;
			if (c == '-') {
				is_negative = true;
				file.get(c);
			}
			bool is_float = is_dot(c);
			string whole;
			string decimal;
			whole.push_back(c);
			while (true) {
				// If we reach an invalid character for an int/float, collect what we have and return it
				char peek = file.peek();
				if ((is_float && !is_numeral(peek)) ||
					!is_float && !is_dot(peek) && !is_numeral(peek)) {
					if (is_float) {
						token.type = Token::Type::_FLOAT;
						
						// If the whole number is just a decimal, we parsed a float that begins with a decimal
						float parsed_float;
						if (whole == ".") {
							parsed_float = stof("." + decimal);
						}
						// Otherwise, whole = "123" and decimal = ".456"
						else {
							float f_whole = stof(whole);
							float f_decimal = stof(decimal);
							parsed_float = f_whole + f_decimal;
						}
						token.float_val = is_negative ? -parsed_float : parsed_float;
						
						return token;
					}
					else {
						token.type = Token::Type::INTEGER;
						token.int_val = stoi(whole);
						return token;
					}
				}
				
				file.get(c);
				
				// We haven't seen a dot yet
				if (!is_float) {
					if (is_dot(c)) {
						is_float = true;
						decimal.push_back(c);
					}
					else if (is_numeral(c)) {
						whole.push_back(c);
					}
				}
				// We've found a dot and are parsing the decimal of the float
				else {
					if (is_numeral(c)) {
						decimal.push_back(c);
					}
					else if (is_dot(c)) {
						string error_msg = "Bad floating point number at line " + to_string(line_number);
						tdns_log.write(error_msg);
						fox_assert(0);
					}
				}
			}
		}
		else if (c == '{') {
			Token token;
			token.type = Token::Type::SYMBOL;
			token.symbol = Symbol::LEFT_BRACKET;
			return token;
		}
		else if (c == '}') {
			Token token;
			token.type = Token::Type::SYMBOL;
			token.symbol = Symbol::RIGHT_BRACKET;
			return token;
		}
		else if (c == '=') {
			Token token;
			token.type = Token::Type::SYMBOL;
			token.symbol = Symbol::EQUALS;
			return token;
		}
		else if (c == ',') {
			Token token;
			token.type = Token::Type::SYMBOL;
			token.symbol = Symbol::COMMA;
			return token;
		}
		else {
			string error_msg = "Found unrecognized token: " + c;
			error_msg += "\nLine number: " + to_string(line_number);
			tdns_log.write(error_msg);
			fox_assert(0);
			Token dummy_for_compiler;
			return dummy_for_compiler;
		}
	}
}
Token Lexer::next_token() {
	fox_assert(tokens.size());
	fox_assert(token_idx < (int)tokens.size());
	if (token_idx == tokens.size()) return tokens[token_idx];
	return tokens[token_idx++];
}
Token Lexer::peek_token() {
	fox_assert(tokens.size());
	fox_assert(token_idx < (int)tokens.size());
	return tokens[token_idx];
}
bool Lexer::is_alpha(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
bool Lexer::is_valid_number_start(char c) {
	bool is_neg_sign = c == '-';
	return is_neg_sign || is_numeral(c) || is_dot(c);
}
bool Lexer::is_numeral(char c) {
	return c >= '0' && c <= '9';
}
bool Lexer::is_dot(char c) {
	return c == '.';
}
bool Lexer::is_underscore(char c) {
	return c == '_';
}
bool Lexer::is_valid_identifier_char(char c) {
	return is_alpha(c) || is_valid_number_start(c) || is_underscore(c) || is_dot(c);
}
bool Lexer::is_whitespace(char c) {
	static bool is_in_comment;
	if (c == '#') is_in_comment = true;
	if (c == '\n') is_in_comment = false;
	if (is_in_comment) return true;
	
	return c == ' ' || c == '\t' || c == '\n';
}

bool TDScript::is_nested_identifier(string& key) {
	for (auto& c : key) {
		if (c == '.') return true;
	}
	
	return false;
}
ASTNode* TDScript::parse(string script_path) {
	if (!lexer.init(script_path)) {
		return nullptr;
	}
	lexer.lex();
	
	KVPNode* kvp;
	while ((kvp = parse_assign())) {
		string& key = kvp->key;
		
		// If we're adding to a table which already exists, put this assignment in that table's node
		if (is_nested_identifier(key)) {
			string key_copy = key;
			char* c_key = (char*)key_copy.c_str();
			char* table_id = strtok(c_key, ".");
			vector<string> keys;
			while (table_id) {
				keys.push_back(table_id);
				table_id = strtok(NULL, ".");
			}
			
			string value_key = keys.back();
			keys.erase(keys.end() - 1);
			
			TableNode* existing_table = global_scope->get_table(keys);
			kvp->key = value_key;
			existing_table->assignments.push_back(kvp);
		}
		else {
			global_scope->assignments.push_back(kvp);
		}
	}
	
	return global_scope;
}
ASTNode* TDScript::parse_table() {
	TableNode* table_node = new TableNode;
	int arr_len = 0; // This is for tables which have unnamed keys (i.e. an array)
	
	// If we immediately read an end bracket, just return the empty table
	Token cur_token = lexer.peek_token();
	if (cur_token.symbol == Symbol::RIGHT_BRACKET) return table_node;
	
	KVPNode* kvp = new KVPNode;
	while ((kvp = parse_assign())) {
		if (kvp->key == "") {
			kvp->key = to_string(arr_len++);
		}
		table_node->assignments.push_back(kvp);
		cur_token = lexer.peek_token();
		
		// No trailing comma
		if (cur_token.symbol == Symbol::RIGHT_BRACKET) {
			return table_node;
		}
		// Yes trailing comma
		if (cur_token.symbol == Symbol::COMMA) {
			lexer.next_token();
			cur_token = lexer.peek_token();
			if (cur_token.symbol == Symbol::RIGHT_BRACKET) {
				return table_node;
			}
		}
	}
	
	// We only reach this statement if the script forgot to close the table
	fox_assert(0);
	return nullptr; // Just for the compiler
}
KVPNode* TDScript::parse_assign() {
	Token cur_token = lexer.next_token();
	if (cur_token.type == Token::Type::_EOF) {
		return nullptr;
	}
	
	auto assign_node = new KVPNode;
	ASTNode* val_node = nullptr;
	
	// An array table assignment that has no key e.g.
	// my_table = {
	//     { my_var = 2 },
	//     my_var = 0
	// }
	if (cur_token.type == Token::SYMBOL && cur_token.symbol == Symbol::LEFT_BRACKET) {
		auto table_node = parse_table();
		cur_token = lexer.next_token();
		if (cur_token.symbol != Symbol::RIGHT_BRACKET) {
			string error_msg = "Unfinished table at line " + to_string(lexer.line_number);
			tdns_log.write(error_msg);
			fox_assert(0);
		}
		
		val_node = table_node;
	}
	else if (cur_token.type == Token::Type::INTEGER) {
		auto integer_node = new IntegerNode; val_node = integer_node;
		integer_node->value = cur_token.int_val;
	}
	else if (cur_token.type == Token::Type::STRING_LITERAL) {
		auto string_literal_node = new StringLiteralNode; val_node = string_literal_node;
		string_literal_node->value = cur_token.str_val;
	}
	else if (cur_token.type == Token::Type::_FLOAT) {
		auto float_node = new FloatNode; val_node = float_node;
		float_node->value = cur_token.float_val;
	}
	else if (cur_token.type == Token::Type::BOOL) {
		auto bool_node = new BoolNode; val_node = bool_node;
		bool_node->value = cur_token.bool_val;
	}
	else if (cur_token.type == Token::Type::SYMBOL) {
		if (cur_token.symbol == Symbol::LEFT_BRACKET) {
			auto table_node = parse_table(); val_node = table_node;
			cur_token = lexer.next_token();
			if (cur_token.symbol != Symbol::RIGHT_BRACKET) {
				string error_msg = "Unfinished table at line " + to_string(lexer.line_number);
				tdns_log.write(error_msg);
				fox_assert(0);
			}
		}
		else {
			int x = 0;
		}
	}
	
	// A regular assignment (i.e. my_var = {bool, int, table, ...})
	else if (cur_token.type == Token::Type::IDENTIFIER) {
		assign_node->key = cur_token.str_val;
		cur_token = lexer.peek_token();
		
		// id = value
		if (cur_token.symbol == Symbol::EQUALS) {
			lexer.next_token();
			cur_token = lexer.next_token();
			
			// Parse out any literals
			if (cur_token.type == Token::Type::INTEGER) {
				auto integer_node = new IntegerNode; val_node = integer_node;
				integer_node->value = cur_token.int_val;
			}
			else if (cur_token.type == Token::Type::STRING_LITERAL) {
				auto string_literal_node = new StringLiteralNode; val_node = string_literal_node;
				string_literal_node->value = cur_token.str_val;
			}
			else if (cur_token.type == Token::Type::_FLOAT) {
				auto float_node = new FloatNode; val_node = float_node;
				float_node->value = cur_token.float_val;
			}
			else if (cur_token.type == Token::Type::BOOL) {
				auto bool_node = new BoolNode; val_node = bool_node;
				bool_node->value = cur_token.bool_val;
			}
			else if (cur_token.type == Token::Type::IDENTIFIER) {
				vector<string> keys;
				char* key = strtok((char*)cur_token.str_val.c_str(), ".");
				while (key) {
					keys.push_back(key);
					key = strtok(NULL, ".");
				}
				val_node = global_scope->get_raw(keys);
				int x = 0;
			}
			else if (cur_token.type == Token::Type::SYMBOL) {
				if (cur_token.symbol == Symbol::LEFT_BRACKET) {
					auto table_node = parse_table(); val_node = table_node;
					cur_token = lexer.next_token();
					if (cur_token.symbol != Symbol::RIGHT_BRACKET) {
						string error_msg = "Unfinished table at line " + to_string(lexer.line_number);
						tdns_log.write(error_msg);
						fox_assert(0);
					}
				}
			}
			else {
				string error_msg = "Malformed assignment on line " + to_string(lexer.line_number);
				tdns_log.write(error_msg);
				fox_assert(0);
			}
			
		}
		// This case happens when referencing an ID in a table used as an array
		else {
			val_node = global_scope->get_raw(keys_from_string(assign_node->key));
			assign_node->key = "";
		}
	}
	else {
		string error_msg = "Expected identifier at line " + to_string(lexer.line_number);
		tdns_log.write(error_msg);
		fox_assert(0);
		return nullptr;
	}
	
	assign_node->value = val_node;
	return assign_node;
}
vector<string> TDScript::keys_from_string(string key_str) {
	char* key = strtok((char*)key_str.c_str(), ".");
	vector<string> keys;
	while (key) {
		keys.push_back(key);
		key = strtok(NULL, ".");
	}
	
	return keys;
}
void TDScript::script_dir(string dir_path) {
	// Always check for the directory's init file first
	script_init_if_exists(dir_path);
	
	for (auto it = directory_iterator(dir_path); it != directory_iterator(); it++) {
		string path = it->path().string();

		// Make sure the path is a TDS file that has not been run
		if (is_regular_file(it->status())) {
			if (is_tds(path)) {
				// Don't double run the init file 
				string maybe_init_file = dir_path + "\\__init__.tds";
				if (string_comp(path, maybe_init_file)) {
					continue;
				}
				
				// If so, script it
				parse(path);
			}
		}
		else if (is_directory(it->status())) {
			script_dir(path);
		}
	}
}

void TDScript::script_init_if_exists(string dir_path) {
	string maybe_init_file = dir_path + "\\__init__.tds";
	ifstream f(maybe_init_file.c_str());
	if (f.good()) {
		parse(maybe_init_file);
	}	
}

void init_tdscript() {
	ScriptManager.global_scope = new TableNode;
	ScriptManager.script_init_if_exists(absolute_path("src\\scripts\\"));
	
	TableNode* files = tds_table("meta", "script_dirs");
	for (uint i = 0; i < files->assignments.size(); i++) {
		string script_dir = tds_string("meta", "script_dirs", to_string(i));
		ScriptManager.script_dir(absolute_path("src\\scripts\\") + script_dir);
	}
}

void test_tdscript() {
	string boonhouse_test = tds_string2(ScriptManager.global_scope, "entity", "boonhouse_door", "components", "Graphic_Component", "Animations", "boonhouse_door", "0");
	string cantina_test = tds_string2(ScriptManager.global_scope, "entity", "picture", "components", "Graphic_Component", "Animations", "picture", "0");
	string intro_police_test = tds_string2(ScriptManager.global_scope, "entity", "intro_police", "scripts", "intro1", "4", "kind");
	string wilson_test = tds_string2(ScriptManager.global_scope, "entity", "wilson", "State_Machine", "main1", "task", "0", "kind");
	
	TableNode* gc = tds_table2(ScriptManager.global_scope, "entity", "wilson", "components", "Graphic_Component");
	auto graphic = new Graphic_Component;
	graphic->init_from_table(gc);
	
	TableNode* movement_table = tds_table2(ScriptManager.global_scope, "entity", "wilson", "components", "Movement_Component");
	auto mc = new Movement_Component;
	mc->init_from_table(movement_table);
	
	TableNode* collision_table = tds_table2(ScriptManager.global_scope, "entity", "wilson", "components", "Collision_Component");
	auto cc = new Collision_Component;
	cc->init_from_table(collision_table);
	
	TableNode* task_table = tds_table2(ScriptManager.global_scope, "entity", "wilson", "scripts", "intro");
	auto task = new Task;
	task->init_from_table(task_table, { -1, nullptr });
	
	ScriptManager.script_dir(absolute_path("src\\scripts\\tds_test"));
	string strval = tds_string2(ScriptManager.global_scope, "strval");
	int intval = tds_int2(ScriptManager.global_scope, "intval");
	float fval = tds_float2(ScriptManager.global_scope, "fval");
	bool bval = tds_bool2(ScriptManager.global_scope, "bval");
	string table2_strval = tds_string2(ScriptManager.global_scope, "table2", "strval");
	int table2_intval = tds_int2(ScriptManager.global_scope, "table2", "intval");
	float table2_fval = tds_float2(ScriptManager.global_scope, "table2", "fval");
	bool table2_bval = tds_bool2(ScriptManager.global_scope, "table2", "bval");
	string table2_nestedstr = tds_string2(ScriptManager.global_scope, "table2", "nested_str");
	int table2_nestedtbl_val = tds_int2(ScriptManager.global_scope, "table2", "nested_tbl", "intval");
	bool table2_nestedbval = tds_bool2(ScriptManager.global_scope, "table2", "nested_bval");
	string array_0 = tds_string2(ScriptManager.global_scope, "array", "0");
	string array_1 = tds_string2(ScriptManager.global_scope, "array", "1");
	int array_2 = tds_int2(ScriptManager.global_scope, "array", "2");
	
	TableNode* some_table = new TableNode;
	some_table->push_back(true);
	tds_set(some_table, "settin_it");
	//update_state("intro_door_answered", true);
}

