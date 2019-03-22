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

struct TableWriter {
	int indent = 0;
	bool _same_line = false;
	vector<string> lines;
	
	string& indent_line(string& line) {
		fox_for(idx, indent) {
			line = "\t" + line;
		}
		
		return line;
	}
	void write_line(string& line) {
		if (!_same_line) {
			lines.push_back(indent_line(line));
		} else {
			string& last_line = lines.back();
			last_line += line;
			_same_line = false;
		}
	}
	void write_line(string&& line) {
		if (!_same_line) {
			lines.push_back(std::move(indent_line(line)));
		} else {
			string& last_line = lines.back();
			last_line += line;
			_same_line = false;
		}
	}
	void same_line() {
		_same_line = true;
	}
	
	void begin_table() {
		string line = "{";
		write_line(line);
		indent++;
	}
	void end_table() {
		indent--;
		string line = "}";
		write_line(line);
	}
	
	void write_int(int i) {
		string line = to_string(i);
		write_line(line);
	}
	
	void dump(string path) {
		ofstream stream(path, ofstream::out | ofstream::trunc);
		for (auto& line : lines) {
			stream << line << endl;
		}
	}
};

// @spader 3/19/19: these should totally be reversed. The one that you want to do
// 80% of the time is the one where you use the global table. 
#define tds_int2(root, ...) root->get_int({__VA_ARGS__})
#define tds_string2(root, ...) root->get_string({__VA_ARGS__})
#define tds_float2(root, ...) root->get_float({__VA_ARGS__})
#define tds_bool2(root, ...) root->get_bool({__VA_ARGS__})
#define tds_table2(root, ...) root->get_table({__VA_ARGS__})
#define tds_raw2(root, ...) root->get_raw({__VA_ARGS__})
#define tds_set2(root, val, ...) root->set({__VA_ARGS__}, val)

#define tds_int(...) ScriptManager.global_scope->get_int({__VA_ARGS__})
#define tds_string(...) ScriptManager.global_scope->get_string({__VA_ARGS__})
#define tds_float(...) ScriptManager.global_scope->get_float({__VA_ARGS__})
#define tds_bool(...) ScriptManager.global_scope->get_bool({__VA_ARGS__})
#define tds_table(...) ScriptManager.global_scope->get_table({__VA_ARGS__})
#define tds_raw(...) ScriptManager.global_scope->get_raw({__VA_ARGS__})
#define tds_set(val, ...) ScriptManager.global_scope->set({__VA_ARGS__}, val)

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
	virtual void dump(TableWriter& output) = 0;
};
struct IntegerNode : ASTNode {
	int value;
	IntegerNode() { type = ANK_IntegerNode; }
	void dump(TableWriter& output) override;
};
struct FloatNode : ASTNode {
	float value;
	FloatNode() { type = ANK_FloatNode; }
	void dump(TableWriter& output) override;
};
struct BoolNode : ASTNode {
	bool value;
	BoolNode() { type = ANK_BoolNode; }
	void dump(TableWriter& output) override;
};
struct StringLiteralNode : ASTNode {
	string value;
	StringLiteralNode() { type = ANK_StringLiteralNode; }
	void dump(TableWriter& output) override;
};
struct KVPNode : ASTNode {
	string key;
	ASTNode* value;
	KVPNode() { type = ANK_AssignNode; key = ""; }
	void dump(TableWriter& output) override;
};
struct TableNode : ASTNode {
	vector<KVPNode*> assignments;
	TableNode() { type = ANK_TableNode; }
	void dump(TableWriter& output) override;
	
	template<typename T>
		Primitive  get(vector<string>& keys);
	int        get_int(vector<string> keys);
	string     get_string(vector<string> keys);
	float      get_float(vector<string> keys);
	bool       get_bool(vector<string> keys);
	TableNode* get_table(vector<string> keys);
	ASTNode*   get_raw(vector<string> keys);
	ASTNode*   maybe_key(string key);
	
	void       set(vector<string> keys, int value);
	void       set(vector<string> keys, string value);
	void       set(vector<string> keys, float value);
	void       set(vector<string> keys, bool value);
	void       set(vector<string> keys, TableNode* value);
	
	template<typename T>                                // Note: Only use this if you know you're actually pushing
		void       push_back(T value);                      // to an array. 
	
	void       dump(string path);
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
	
	bool init(string script_path);
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
	KVPNode* parse_assign();
	vector<string> keys_from_string(string key_str);
	void script_file(string script_path);
	void script_dir(string dir_path);
};
TDScript ScriptManager;
