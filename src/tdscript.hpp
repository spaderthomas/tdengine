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

#define tds_int(root, ...) root->get_int({__VA_ARGS__})
#define tds_string(root, ...) root->get_string({__VA_ARGS__})
#define tds_float(root, ...) root->get_float({__VA_ARGS__})
#define tds_bool(root, ...) root->get_bool({__VA_ARGS__})
#define tds_table(root, ...) root->get_table({__VA_ARGS__})
#define tds_raw(root, ...) root->get_raw({__VA_ARGS__})

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
struct KVPNode : ASTNode {
	string key;
	ASTNode* value;
	KVPNode() { type = ANK_AssignNode; key = ""; }
	void im_a_real_boy() override {};
};
struct TableNode : ASTNode {
	vector<ASTNode*> assignments;
	TableNode() { type = ANK_TableNode; }
	void im_a_real_boy() override {}

	template<typename T>
	Primitive get(vector<string>& keys) {
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
	ASTNode* get_raw(vector<string> keys) {
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
	ASTNode* maybe_key(string key) {
		for (auto node : assignments) {
			KVPNode* kvp = (KVPNode*)node;
			if (kvp->key == key) return kvp;
		}

		return nullptr;
	}
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

	void init(string script_path);
	void lex();
	Token next_token_internal();

	Token next_token();
	Token peek_token();

	bool is_alpha(char c);
	bool is_valid_number_start(char c);
	bool is_numeral(char c);
	bool is_dot(char c);
	bool is_underscore(char c);
	bool is_valid_identifier_char(char c);
	bool is_whitespace(char c);
};

struct TDScript {
	Lexer lexer;
	TableNode* global_scope = nullptr;

	bool is_nested_identifier(string& key);
	ASTNode* parse(string script_path);
	ASTNode* parse_table();
	ASTNode* parse_assign();
	vector<string> keys_from_string(string key_str);
	void script_file(string script_path);
	void script_dir(string dir_path);
};
TDScript ScriptManager;