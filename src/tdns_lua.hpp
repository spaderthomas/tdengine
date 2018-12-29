

// AST nodes
enum ASTNodeType {
	ANK_StringLiteralNode,
	ANK_IntegerNode,
	ANK_FloatNode,
	ANK_BoolNode,
	ANK_AssignNode,
	ANK_TableNode,
};
struct ASTNode {
	ASTNodeType type;
	virtual void im_a_real_boy() = 0;
};
struct IntegerNode : ASTNode {
	int value;
	IntegerNode() { type = ANK_IntegerNode; }
	void im_a_real_boy() override {};
};
struct FloatNode : ASTNode {
	float value;
	FloatNode() { type = ANK_FloatNode; }
	void im_a_real_boy() override {};
};
struct BoolNode : ASTNode {
	bool value;
	BoolNode() { type = ANK_BoolNode; }
	void im_a_real_boy() override {};
};
struct StringLiteralNode : ASTNode {
	string value;
	StringLiteralNode() { type = ANK_StringLiteralNode; }
	void im_a_real_boy() override {};
};
struct AssignNode : ASTNode {
	string key;
	ASTNode* value;
	AssignNode() { type = ANK_AssignNode; key = ""; }
	void im_a_real_boy() override {};
};
struct TableNode : ASTNode {
	vector<ASTNode*> assignments;
	TableNode() { type = ANK_TableNode; }
	
	void im_a_real_boy() override {};
};


// Lexing
enum Symbol {
	LEFT_BRACKET,
	RIGHT_BRACKET,
	EQUALS,
	COMMA
};
struct Token {
	enum Type {
		SYMBOL,
		IDENTIFIER,
		STRING_LITERAL,
		INTEGER,
		_FLOAT,
		BOOL,
		_EOF
	} type;

	string str_val;
	int int_val;
	float float_val;
	bool bool_val;
	Symbol symbol;
};
struct Lexer {
	ifstream file;
	int line_number = 1;
	vector<Token> tokens;
	int token_idx;

	void init(string script_path) {
		string full_path = absolute_path(script_path);
		file = ifstream(full_path);
		fox_assert(file.good());

		tokens.clear();
		line_number = 1;
		token_idx = 0;
	}

	void lex() {
		token_idx = 0;
		while (true) {
			Token current_token = next_token_internal();
			tokens.push_back(current_token);
			if (current_token.type == Token::Type::_EOF) {
				return;
			}
		}
	}
	Token next_token_internal() {
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
			else if (is_numeric(c)) {
				Token token;

				bool is_float = false;
				string whole;
				string decimal;
				whole.push_back(c);
				while (true) {
					// If we reach an invalid character for an int/float, collect what we have and return it
					char peek = file.peek();
					if ((is_float && !is_numeric(peek)) || !is_float && !is_dot(peek) && !is_numeric(peek)) {
						if (is_float) {
							token.type = Token::Type::_FLOAT;
							float f_whole = stof(whole);
							float f_decimal = stof(decimal);
							token.float_val = f_whole + f_decimal;
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
						else if (is_numeric(c)) {
							whole.push_back(c);
						}
					}
					// We've found a dot and are parsing the decimal of the float
					else {
						if (is_numeric(c)) {
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


	Token next_token() {
		fox_assert(tokens.size());
		fox_assert(token_idx < (int)tokens.size());
		if (token_idx == tokens.size()) return tokens[token_idx];
		return tokens[token_idx++];
	}
	Token peek_token() {
		fox_assert(tokens.size());
		fox_assert(token_idx < (int)tokens.size());
		return tokens[token_idx];
	}

	// Utilities
	bool is_alpha(char c) {
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	}
	bool is_numeric(char c) {
		return c >= '0' && c <= '9';
	}
	bool is_dot(char c) {
		return c == '.';
	}
	bool is_underscore(char c) {
		return c == '_';
	}
	bool is_valid_identifier_char(char c) {
		return is_alpha(c) || is_numeric(c) || is_underscore(c) || is_dot(c);
	}
	bool is_whitespace(char c) {
		static bool is_in_comment;
		if (c == '#') is_in_comment = true;
		if (c == '\n') is_in_comment = false;
		if (is_in_comment) return true;

		return c == ' ' || c == '\t' || c == '\n';
	}
};


// Parsing and interface
struct Primitive {
	enum Type {
		PRIM_ERROR,
		STRING,
		INTEGER,
		BOOL,
		FLOAT
	} type;

	union PrimitiveUnion {
		const char* strval;
		int intval;
		float floatval;
		bool boolval;
	} values;
};

struct {
	Lexer lexer;
	TableNode* global_scope = nullptr;

	bool is_nested_identifier(string& key) {
		for (auto& c : key) {
			if (c == '.') return true;
		}

		return false;
	}
	ASTNode* parse(string script_path) {
		lexer.init(script_path);
		lexer.lex();

		if (!global_scope) global_scope = new TableNode;

		ASTNode* assignment;
		while ((assignment = parse_assign())) {
			string& key = ((AssignNode*)assignment)->key;

			// If we're adding to a table which already exists, put this assignment in that table's node
			if (is_nested_identifier(key)) {
				char* c_key = (char*)key.c_str();
				char* table_id = strtok(c_key, ".");
				vector<string> keys;
				while (table_id) {
					keys.push_back(table_id);
					table_id = strtok(NULL, ".");
				}

				string value_key = keys.back();
				keys.erase(keys.end() - 1);

				TableNode* existing_table = get_table(keys);
				((AssignNode*)assignment)->key = value_key;
				existing_table->assignments.push_back(assignment);
			}
			else {
				global_scope->assignments.push_back(assignment);
			}
		}

		return global_scope;
	}
	ASTNode* parse_table() {
		TableNode* table_node = new TableNode;
		int arr_len = 0; // This is for tables which have unnamed keys (i.e. an array)

		// If we immediately read an end bracket, just return the empty table
		Token cur_token = lexer.peek_token();
		if (cur_token.symbol == Symbol::RIGHT_BRACKET) return table_node;

		ASTNode* assign_node = new AssignNode;
		while ((assign_node = parse_assign())) {
			if (((AssignNode*)assign_node)->key == "") {
				((AssignNode*)assign_node)->key = to_string(arr_len++);
			}
			table_node->assignments.push_back(assign_node);
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
	ASTNode* parse_assign() {
		Token cur_token = lexer.next_token();
		if (cur_token.type == Token::Type::_EOF) {
			return nullptr;
		}

		auto assign_node = new AssignNode;
		ASTNode* val_node = nullptr;

		// An array table assignment that has no key e.g.
		// my_table = {
		//     { my_var = 2 },
		//     my_var = 0
		// }
		if (cur_token.symbol == Symbol::LEFT_BRACKET) {
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
		}

		// A regular assignment (i.e. my_var = {bool, int, table, ...})
		else if (cur_token.type == Token::Type::IDENTIFIER) {
			assign_node->key = cur_token.str_val;
			cur_token = lexer.next_token();
			if (cur_token.symbol == Symbol::EQUALS) {
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
			else {
				string error_msg = "Found identifier without assignment on line " + to_string(lexer.line_number);
				tdns_log.write(error_msg);
				fox_assert(0);
				return nullptr; // Just for the compiler
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

	void script_file(string script_path) {
		ASTNode* ast = parse(script_path);
	}
	void script_dir(string dir_path) {
		// Always check for the directory's init file first
		string maybe_init_file = dir_path + "\\init.tds";
		ifstream f(maybe_init_file.c_str());
		if (f.good()) {
			script_file(maybe_init_file);
		}

		for (auto it = directory_iterator(dir_path); it != directory_iterator(); it++) {
			string path = it->path().string();
			if (is_regular_file(it->status())) {
				if (is_tds(path)) {
					// Don't double run the init file 
					if (!path.compare(maybe_init_file)) {
						continue;
					}
					script_file(path);
				}
			}
			else if (is_directory(it->status())) {
				script_dir(path);
			}
		}
	}

	template<typename T>
	Primitive get(vector<string> keys) {
		string value_key = keys.back();
		keys.erase(keys.end() - 1);
		TableNode* containing_table = get_table(keys);
		fox_assert(containing_table);
		
		for (auto& node : containing_table->assignments) {
			AssignNode* assignment = (AssignNode*)node;
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
				fox_assert(payload->type == ASTNodeType::ANK_FloatNode);
				prim.type = Primitive::Type::FLOAT;
				prim.values.floatval = ((FloatNode*)payload)->value;
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

	int get_int(vector<string> keys) {
		Primitive prim = get<int>(keys);
		fox_assert(prim.type == Primitive::Type::INTEGER);
		return prim.values.intval;
	}
	string get_string(vector<string> keys) {
		Primitive prim = get<string>(keys);
		fox_assert(prim.type == Primitive::Type::STRING);
		return prim.values.strval;
	}
	float get_float(vector<string> keys) {
		Primitive prim = get<float>(keys);
		fox_assert(prim.type == Primitive::Type::FLOAT);
		return prim.values.floatval;
	}
	bool get_bool(vector<string> keys) {
		Primitive prim = get<bool>(keys);
		fox_assert(prim.type == Primitive::Type::BOOL);
		return prim.values.boolval;
	}
	TableNode* get_table(vector<string> keys) {
		if (!keys.size()) return global_scope;

		TableNode* current_scope = global_scope;
		string error_msg = "KeyError: global_scope"; // Write this here so that we know exactly what key we failed at
		for (int key_idx = 0; key_idx < (int)keys.size(); key_idx++) {
			auto& key = keys[key_idx];

			bool found = false;
			for (ASTNode* node : current_scope->assignments) {
				fox_assert(node->type == ASTNodeType::ANK_AssignNode);

				AssignNode* assignment = (AssignNode*)node;
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

} TDScript;
#define tds_int(...) TDScript.get_int({__VA_ARGS__})
#define tds_string(...) TDScript.get_string({__VA_ARGS__})
#define tds_float(...) TDScript.get_float({__VA_ARGS__})
#define tds_bool(...) TDScript.get_bool({__VA_ARGS__})
#define tds_table(...) TDScript.get_table({__VA_ARGS__})

struct LuaState {
	sol::state state;

	void recursive_add_dir(string dir) {
		// Always check for the directory's init file first
		string maybe_init_file = dir + "\\init.lua";
		ifstream f(maybe_init_file.c_str());
		if (f.good()) {
			state.script_file(maybe_init_file);
		}

		for (auto it = directory_iterator(dir); it != directory_iterator(); it++) {
			string path = it->path().string();
			if (is_regular_file(it->status())) {
				if (is_lua(path)) {
					// Don't double run the init file 
					if (!path.compare(maybe_init_file)) {
						continue;
					}
					state.script_file(path);
				}
			}
			else if (is_directory(it->status())) {
				recursive_add_dir(path);
			}
		}
	}

	void init() {
		state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io, sol::lib::package, sol::lib::math, sol::lib::table);
		bind_functions();

		// Userdata
		state.new_usertype<EntityHandle>(
			"EntityHandle"
			);

		state.new_usertype<Entity>(
			"Entity",
			"get_component", &Entity::get_component,
			"lua_id", &Entity::lua_id
			);

		state.new_usertype<Level>(
			"Level",
			"name", &Level::name,
			"load", &Level::load,
			"draw", &Level::draw,
			"entities", &Level::entities
			);


		// Load scripts
		state.script_file(absolute_path("src\\scripts\\meta.lua"));
		state.script_file(absolute_path("src\\scripts\\init.lua"));
		sol::table files = state["meta"]["scripts"];

		// Load everything that needs to be in the global namespace from scripts
		for (auto& kvp : files) {
			sol::table file = kvp.second;
			string script = file["name"];
			if (script == "init.lua") continue;

			if (file["is_dir"]) {
				recursive_add_dir(absolute_path("src\\scripts\\") + script);
				continue;
			}

			string path = absolute_path("src\\scripts\\") + script + ".lua";
			state.script_file(path);
		}
	}

	void init_after_load() {
		sol::table after_load = state["meta"]["after_load"];
		for (auto& kvp : after_load) {
			sol::function func = kvp.second;
			func();
		}
	}
	void run_game_update(float dt) {
		//@hack either a better way to find the function with middleclass or dont use middleclass
		sol::function lua_update = state["Game"]["__declaredMethods"]["update"];
		lua_update(state["game"], dt);
	}
	sol::table entity_table() {
		return state["entity"];
	}
	Level* get_active_level() {
		return state["game"]["level"];
	}
	void set_active_level(Level* level) {
		state["game"]["level"] = level;
	}
	EntityHandle get_hero() {
		return state["game"]["hero"];
	}
	sol::table get_task(string entity, string scene) {
		return state["entity"][entity]["scripts"][scene];
	}
};
LuaState Lua;

void init_tdscript() {
	TDScript.script_file("src\\scripts\\meta.tds");
	TableNode* files = tds_table("meta", "scripts");
	for (uint i = 0; i < files->assignments.size(); i++) {
		TableNode* script = tds_table("meta", "scripts", to_string(i));
		string name = 
	}
}

void test_tdscript() {
	TDScript.script_file("src\\scripts\\tds_test\\test2.tds");
	TDScript.script_file(absolute_path("src\\scripts\\meta.tds"));
	string meta_scripts_name = tds_string("meta", "scripts", "0", "name");

	TDScript.script_dir(absolute_path("src\\scripts\\tds_test"));
	bool my_init = tds_bool("my_init");

	//TDScript.script_file("src\\scripts\\test.tds");
	string strval = tds_string("strval");
	int intval = tds_int("intval");
	float fval = tds_float("fval");
	bool bval = tds_bool("bval");
	string table2_strval = tds_string("table2", "strval");
	int table2_intval = tds_int("table2", "intval");
	float table2_fval = tds_float("table2", "fval");
	bool table2_bval = tds_bool("table2", "bval");
	string table2_nestedstr = tds_string("table2", "nested_str");
	int table2_nestedtbl_val = tds_int("table2", "nested_tbl", "intval");
	bool table2_nestedbval = tds_bool("table2", "nested_bval");
	string array_0 = tds_string("array", "0");
	string array_1 = tds_string("array", "1");
	int array_2 = tds_int("array", "2");
	TDScript.script_file("src\\scripts\\test2.tds");
	int table_intval = tds_int("table", "intval");

	int x = 0;
}

