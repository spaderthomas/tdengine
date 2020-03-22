// Surely there is a better way to not re-write all this code, but, hey, it works
void TableNode::set(std::vector<std::string> keys, int value) {
	// Get the table that we want to write to
	std::string value_key = keys.back();
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
void TableNode::set(std::vector<std::string> keys, std::string value) {
	// Get the table that we want to write to
	std::string value_key = keys.back();
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
void TableNode::set(std::vector<std::string> keys, float value) {
	// Get the table that we want to write to
	std::string value_key = keys.back();
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
void TableNode::set(std::vector<std::string> keys, bool value) {
	// Get the table that we want to write to
	std::string value_key = keys.back();
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
void TableNode::set(std::vector<std::string> keys, TableNode* value) {
	// Get the table that we want to write to
	std::string value_key = keys.back();
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

void TableNode::del(std::vector<std::string> keys) {
	std::string final_key = keys.back();
	keys.erase(keys.end() - 1);
	auto not_found_error = [final_key, keys]() mutable -> void {
		std::string error_msg = "Tried to delete ";
		error_msg += keys.front();
		keys.erase(keys.begin());
		for (auto& key : keys) {
			error_msg += "[" + key + "]";
		}
		error_msg += "[" + final_key + "]";
		error_msg += ", but it did not exist!";
		tdns_log.write(error_msg);
		fox_assert(0);
	};
	
	TableNode* containing_table = get_table(keys);
	if (!containing_table) not_found_error();

	bool found = false;
	for (auto it = containing_table->assignments.begin(); it != containing_table->assignments.end(); ++it) {
		KVPNode* kvp = *it;
		if (kvp->key == final_key) {
			containing_table->assignments.erase(it);
			found = true;
			break;
		}
	}

	if (!found) not_found_error();
}

template<typename T>      
void TableNode::push_back(T value) {
	std::vector<std::string> keys = { std::to_string(assignments.size()) };
	this->set(keys, value);
}

template<typename T>
Primitive TableNode::get(std::vector<std::string>& keys) {
	std::string value_key = keys.back();
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
		else if (typeid(T) == typeid(std::string)) {
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
int TableNode::get_int(std::vector<std::string> keys) {
	Primitive prim = get<int>(keys);
	fox_assert(prim.type == Primitive::Type::INTEGER);
	return prim.values.intval;
}
std::string TableNode::get_string(std::vector<std::string> keys) {
	Primitive prim = get<std::string>(keys);
	fox_assert(prim.type == Primitive::Type::STRING);
	return prim.values.strval;
}
float TableNode::get_float(std::vector<std::string> keys) {
	Primitive prim = get<float>(keys);
	fox_assert(prim.type == Primitive::Type::FLOAT);
	return prim.values.floatval;
}
bool TableNode::get_bool(std::vector<std::string> keys) {
	Primitive prim = get<bool>(keys);
	fox_assert(prim.type == Primitive::Type::BOOL);
	return prim.values.boolval;
}
TableNode* TableNode::get_table(std::vector<std::string> keys) {
    if (!keys.size()) return this;
	
	TableNode* current_scope = this;
	std::string error_msg = "KeyError: global_scope"; // Write this here so that we know exactly what key we failed at
	for (uint key_idx = 0; key_idx < keys.size(); key_idx++) {
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
ASTNode* TableNode::get_raw(std::vector<std::string> keys) {
	if (!keys.size()) return this;
	
	TableNode* current_scope = this;
	std::string error_msg = "KeyError: global_scope"; // Write this here so that we know exactly what key we failed at
	for (uint key_idx = 0; key_idx < keys.size(); key_idx++) {
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
ASTNode* TableNode::maybe_key(std::string key) {
	for (auto node : assignments) {
		KVPNode* kvp = (KVPNode*)node;
		if (kvp->key == key) return kvp;
	}
	
	return nullptr;
}
bool TableNode::has_key(std::string key) {
	return maybe_key(key);
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
		std::string defn = key + " = ";
		output.write_line(defn);
		output.same_line();
	}
	
	value->dump(output);
}

void IntegerNode::dump(TableWriter& output) {
	output.write_line(std::to_string(value));
}
void FloatNode::dump(TableWriter& output) {
	output.write_line(std::to_string(value));
}
void BoolNode::dump(TableWriter& output) {
	output.write_line(std::to_string(value));
}
void StringLiteralNode::dump(TableWriter& output) {
	std::string as_literal = "\"" + value + "\"";
	output.write_line(as_literal);
}

bool Lexer::init(std::string script_path) {
	file = std::ifstream(script_path);
	file_path = script_path;
	if (!file.good()) {
		tdns_log.write("File was not good: " + script_path);
		fox_assert(file.good());
	}
	file.seekg(0, std::ios_base::end);
	auto file_size = file.tellg();
	file.seekg(0, std::ios_base::beg);
	
	tokens.clear();
	line_number = 1;
	token_idx = 0;
	
	if (!file_size) {
		tdns_log.write(std::string("Script_path ") + script_path + " is empty.");
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
			std::string whole;
			std::string decimal;
			whole.push_back(c);
			while (true) {
				// If we reach an invalid character for an int/float, collect what we have and return it
				char peek = file.peek();
				if ((is_float && !is_numeral(peek)) ||
					(!is_float && !is_dot(peek) && !is_numeral(peek))) {
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
						std::string error_msg = "Bad floating point number at line " + std::to_string(line_number) + " of file " + file_path;
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
			std::string i_hate_cpp;
			i_hate_cpp += c;
			std::string error_msg = "Found unrecognized token: " + i_hate_cpp;
			error_msg += "\nLine number: " + std::to_string(line_number) + " of file " + file_path;;
			tdns_log.write(error_msg);
			fox_assert(0);
			Token dummy_for_compiler;
			return dummy_for_compiler;
		}
	}
}
Token Lexer::next_token() {
	fox_assert(tokens.size());
	fox_assert(token_idx < tokens.size());
	if (token_idx == tokens.size()) return tokens[token_idx];
	return tokens[token_idx++];
}
Token Lexer::peek_token() {
	fox_assert(tokens.size());
	fox_assert(token_idx < tokens.size());
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

bool TDScript::is_nested_identifier(std::string& key) {
	for (auto& c : key) {
		if (c == '.') return true;
	}
	
	return false;
}
ASTNode* TDScript::parse(std::string script_path) {
	normalize_path(script_path);
	tdns_log.write("Scripted file: " + script_path, Log_Flags::File);
	
	if (!lexer.init(script_path)) {
		return nullptr;
	}
	lexer.lex();
	
	KVPNode* kvp;
	while ((kvp = parse_assign())) {
		kvp->file = script_path;
		std::string old = kvp->key;
		std::string key = kvp->key;
		TableNode* containing_table = global_scope;
		
		// If we're adding to a table which already exists, find that table
		if (is_nested_identifier(key)) {
			std::string key_copy = key;
			char* c_key = (char*)key_copy.c_str();
			char* table_id = strtok(c_key, ".");
			std::vector<std::string> keys;
			while (table_id) {
				keys.push_back(table_id);
				table_id = strtok(NULL, ".");
			}
			
			std::string value_key = keys.back();
			keys.erase(keys.end() - 1);
			
			containing_table = global_scope->get_table(keys);

			// The node itself needs to have the updated key (e.g "stuff" instead of "nested.my.stuff")
			kvp->key = value_key;
			key = value_key;
		}

		for (auto it = containing_table->assignments.begin(); it != containing_table->assignments.end(); it++) {
			KVPNode* existing_kvp = *it;
			if (existing_kvp->key == key) {
				containing_table->assignments.erase(it);
				break;
			}
		}

		containing_table->assignments.push_back(kvp);
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
			kvp->key = std::to_string(arr_len++);
		}
		table_node->assignments.push_back(kvp);
		cur_token = lexer.peek_token();
		
		// No trailing comma
		if (cur_token.type == Token::Type::SYMBOL && cur_token.symbol == Symbol::RIGHT_BRACKET) {
			return table_node;
		}
		// Yes trailing comma
		if (cur_token.type == Token::Type::SYMBOL && cur_token.symbol == Symbol::COMMA) {
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
			std::string error_msg = "Unfinished table at line " + std::to_string(lexer.line_number) + " of file " + lexer.file_path;;
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
				std::string error_msg = "Unfinished table at line " + std::to_string(lexer.line_number) + " of file " + lexer.file_path;;
				tdns_log.write(error_msg);
				fox_assert(0);
			}
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
				std::vector<std::string> keys;
				char* key = strtok((char*)cur_token.str_val.c_str(), ".");
				while (key) {
					keys.push_back(key);
					key = strtok(NULL, ".");
				}
				val_node = global_scope->get_raw(keys);
			}
			else if (cur_token.type == Token::Type::SYMBOL) {
				if (cur_token.symbol == Symbol::LEFT_BRACKET) {
					auto table_node = parse_table(); val_node = table_node;
					cur_token = lexer.next_token();
					if (cur_token.symbol != Symbol::RIGHT_BRACKET) {
						std::string error_msg = "Unfinished table at line " + std::to_string(lexer.line_number) + " of file " + lexer.file_path;;
						tdns_log.write(error_msg);
						fox_assert(0);
					}
				}
			}
			else {
				std::string error_msg = "Malformed assignment on line " + std::to_string(lexer.line_number) + " of file " + lexer.file_path;;
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
		std::string error_msg = "Expected identifier at line " + std::to_string(lexer.line_number) + " of file " + lexer.file_path;;
		tdns_log.write(error_msg);
		fox_assert(0);
		return nullptr;
	}
	
	assign_node->value = val_node;
	return assign_node;
}
std::vector<std::string> TDScript::keys_from_string(std::string key_str) {
	char* key = strtok((char*)key_str.c_str(), ".");
	std::vector<std::string> keys;
	while (key) {
		keys.push_back(key);
		key = strtok(NULL, ".");
	}
	
	return keys;
}

// @spader 10/14/2019: why 
void TDScript::script_file(std::string dir_path) {
	parse(dir_path);
}


void TDScript::script_dir(std::string dir_path) {
	// Always check for the directory's init file first
	script_init_if_exists(dir_path);
	
	for (auto it = directory_iterator(dir_path); it != directory_iterator(); it++) {
		std::string path = it->path().string();
		normalize_path(path);


		// Make sure the path is a TDS file that has not been run
		if (is_regular_file(it->status())) {
			if (is_tds(path)) {
				// Don't double run the init file 
				std::string maybe_init_file = dir_path + "/__init__.tds";
				normalize_path(maybe_init_file);
				if (are_strings_equal(path, maybe_init_file)) {
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

void TDScript::script_init_if_exists(std::string dir_path) {
	std::string maybe_init_file = dir_path + "/__init__.tds";
	std::ifstream f(maybe_init_file.c_str());
	if (f.good()) {
		parse(maybe_init_file);
	}	
}

void init_tdscript() {
	ScriptManager.global_scope = new TableNode;
	ScriptManager.script_init_if_exists(absolute_path("src/scripts"));
	
	TableNode* files = tds_table("meta", "script_dirs");
	for (uint i = 0; i < files->assignments.size(); i++) {
		std::string script_dir = tds_string("meta", "script_dirs", std::to_string(i));
		ScriptManager.script_dir(absolute_path("src/scripts/") + script_dir);
	}
}


