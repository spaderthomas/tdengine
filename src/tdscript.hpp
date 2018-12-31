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
	Primitive  get(vector<string>& keys);
	int        get_int(vector<string> keys);
	string     get_string(vector<string> keys);
	float      get_float(vector<string> keys);
	bool       get_bool(vector<string> keys);
	TableNode* get_table(vector<string> keys);
	ASTNode*   get_raw(vector<string> keys);
	ASTNode*   maybe_key(string key);
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