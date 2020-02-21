struct Shader {
	std::string name;
	uint id;
	uint num_uniforms;
	std::vector<std::string> uniforms_set_this_call;
	
	static int active;

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

Shader textured_shader;
Shader highlighted_shader;
Shader solid_shader;
Shader text_shader;
void init_shaders();
