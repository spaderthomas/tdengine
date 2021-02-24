struct Shader {
	std::string name;
	std::string vs_path;
	std::string fs_path;
	uint id;
	uint num_uniforms;
	std::vector<std::string> uniforms_set_this_call;
	
	static int active;
	static char compilation_status[512];

	void init(std::string vs_path, std::string fs_path, std::string name);
	unsigned int get_uniform_loc(const char* name);

	void set_vec4(const char* name, glm::vec4 vec);
	void set_vec3(const char* name, glm::vec3 vec);
	void set_vec2(const char* name, glm::vec2 vec);
	void set_mat3(const char* name, glm::mat3 mat);
	void set_mat4(const char* name, glm::mat4 mat);
	void set_int(const char* name, GLint val);
	void set_float(const char* name, GLfloat val);

	void begin();
	void check();
	void end();
};
int Shader::active = -1;
char Shader::compilation_status[512];

void init_shaders();

struct ShaderManager {
	std::map<std::string, Shader> shaders;
	
	void add(std::string vs_path, std::string fs_path, std::string name);
	Shader* get(const char* name);
};
ShaderManager& get_shader_manager();	
