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


enum ASTNodeType {
	ANK_StringLiteralNode,
	ANK_IntegerNode,
	ANK_FloatNode,
	ANK_AssignNode,
	ANK_TableNode,
};
struct ASTNode {
	ASTNodeType type;
};
struct IntegerNode : ASTNode {
	int value;
	IntegerNode() { type = ANK_IntegerNode; }
};
struct FloatNode : ASTNode {
	float value;
	FloatNode() { type = ANK_FloatNode; }
};
struct StringLiteralNode : ASTNode {
	string value;
	StringLiteralNode() { type = ANK_StringLiteralNode; }
};
struct AssignNode : ASTNode {
	string key;
	ASTNode* value;
	AssignNode() { type = ANK_AssignNode; }
};
struct TableNode : ASTNode {
	vector<ASTNode*> assignments;
	TableNode() { type = ANK_TableNode; }
};

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
		_EOF
	} type;

	string str_val;
	int int_val;
	float float_val;
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

			// Scan for identifiers
			if (is_alpha(c)) {
				Token token;
				token.type = Token::Type::IDENTIFIER;
				token.str_val = c;

				while (is_valid_identifier_char(file.peek())) {
					token.str_val.push_back(file.get());
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
		fox_assert(token_idx < tokens.size());
		if (token_idx == tokens.size()) return tokens[token_idx];
		return tokens[token_idx++];
	}
	Token peek_token() {
		fox_assert(tokens.size());
		fox_assert(token_idx < tokens.size());
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
		return is_alpha(c) || is_numeric(c) || is_underscore(c);
	}
	bool is_whitespace(char c) {
		return c == ' ' || c == '\t' || c == '\n';
	}

};

struct Parser {
	Lexer lexer;

	ASTNode* parse(string script_path) {
		lexer.init(script_path);
		lexer.lex();

		TableNode* root = new TableNode;
		ASTNode* assignment;
		while ((assignment = parse_assign())) {
			root->assignments.push_back(assignment);
		}

		return root;
	}

	ASTNode* parse_table() {
		TableNode* table_node = new TableNode;
		ASTNode* assign_node = new AssignNode;
		while ((assign_node = parse_assign())) {
			table_node->assignments.push_back(assign_node);
			Token cur_token = lexer.peek_token();

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

		// In the case that parse_assign() returns nullptr, but there's no closing bracket, 
		// it means that we forgot to close the table 
		return nullptr;
	}

	ASTNode* parse_assign() {
		Token cur_token = lexer.next_token();
		if (cur_token.type == Token::Type::_EOF) {
			return nullptr;
		}

		auto assign_node = new AssignNode;
		assign_node->key = cur_token.str_val;
		cur_token = lexer.next_token();
		if (cur_token.symbol == Symbol::EQUALS) {
			cur_token = lexer.next_token();
			ASTNode* val_node = nullptr;

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

			assign_node->value = val_node;
			return assign_node;
		}
		else {
			string error_msg = "Found identifier without assignment on line " + to_string(lexer.line_number);
			tdns_log.write(error_msg);
			fox_assert(0);
			return nullptr;
		}
	}
};


void test_tdscript() {
	Parser p;
	ASTNode* ast = p.parse("src\\scripts\\test.tds");
	int x = 0;
}

