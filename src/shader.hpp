struct Shader {
	string name;
	unsigned int id;
	int num_uniforms;
	vector<string> uniforms_set_this_call;
	
	static int active;

	void init(const char* vs_path, const char* fs_path) {
		auto error = glGetError();

		const char* paths[] = {
			vs_path,
			fs_path
		};

		unsigned int shader_program;
		shader_program = glCreateProgram();
		error = glGetError();

		fox_for(ishader, 2) {
			// Read in shader data
			const char* path = paths[ishader];
			FILE *shader_source_file = fopen(path, "rb");
			fseek(shader_source_file, 0, SEEK_END);
			unsigned int fsize = ftell(shader_source_file);
			fseek(shader_source_file, 0, SEEK_SET);

			char *source = (char*)malloc(fsize + 1);
			fread(source, fsize, 1, shader_source_file);
			source[fsize] = 0;
			fclose(shader_source_file);

			// Compile the shader
			unsigned int shader_kind = (ishader == 0) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
			unsigned int shader = glCreateShader(shader_kind);
			error = glGetError();

			glShaderSource(shader, 1, &source, NULL);
			error = glGetError();

            glCompileShader(shader);
			error = glGetError();
            glAttachShader(shader_program, shader);
			error = glGetError();
		}
		
		// Link into a shader program
		glLinkProgram(shader_program);
		error = glGetError();
		id = shader_program;
		glGetProgramiv(shader_program, GL_ACTIVE_UNIFORMS, &num_uniforms);
		name = name_from_full_path(string(vs_path));
	}

	unsigned int get_uniform_loc(const char* name) {
		auto loc = glGetUniformLocation(id, name);
		if (loc == -1) {
			tdns_log.write("Tried to get uniform location, but it didn't exist. Shader name: " + string(name));
			exit(1);
		}
		return loc;
	}

	void set_vec4(const char* name, glm::vec4& vec) {
		glUniform4f(get_uniform_loc(name), vec.x, vec.y, vec.z, vec.w);
		if (!tdns_find(uniforms_set_this_call, name)) {
			uniforms_set_this_call.push_back(name);
		}
	}
	void set_vec3(const char* name, glm::vec3& vec) {
		glUniform3f(get_uniform_loc(name), vec.x, vec.y, vec.z);
		if (!tdns_find(uniforms_set_this_call, name)) {
			uniforms_set_this_call.push_back(name);
		}
	}
	void set_vec2(const char* name, glm::vec2& vec) {
		glUniform2f(get_uniform_loc(name), vec.x, vec.y);
		if (!tdns_find(uniforms_set_this_call, name)) {
			uniforms_set_this_call.push_back(name);
		}
	}
	void set_mat3(const char* name, glm::mat3& mat) {
		glUniformMatrix3fv(get_uniform_loc(name), 1, GL_FALSE, glm::value_ptr(mat));
		if (!tdns_find(uniforms_set_this_call, name)) {
			uniforms_set_this_call.push_back(name);
		}
	}
	void set_mat4(const char* name, glm::mat4& mat) {
		glUniformMatrix4fv(get_uniform_loc(name), 1, GL_FALSE, glm::value_ptr(mat));
		if (!tdns_find(uniforms_set_this_call, name)) {
			uniforms_set_this_call.push_back(name);
		}
	}
	void set_int(const char* name, GLint val) {
		glUniform1i(get_uniform_loc(name), val);
		if (!tdns_find(uniforms_set_this_call, name)) {
			uniforms_set_this_call.push_back(name);
		}
	}
	void set_float(const char* name, GLfloat val) {
		glUniform1f(get_uniform_loc(name), val);
		if (!tdns_find(uniforms_set_this_call, name)) {
			uniforms_set_this_call.push_back(name);
		}
	}

	void begin() {
		if (Shader::active != -1) {
			tdns_log.write("Called begin() with shader: " + name + ", but there was already an active shader. Did you forget to call end()?");
			exit(1);
		}
		glUseProgram(id);
		Shader::active = id;
		uniforms_set_this_call.clear();
	}

	void check() {
		if (uniforms_set_this_call.size() != num_uniforms) {
			string msg = "You didn't fill in all of the uniforms! Shader was: " + string(name);
			tdns_log.write(msg);
			exit(1);
		}
		if (Shader::active != id) {
			string msg = "Checked shader before draw, but it was not set. Did you forget to call begin()? Shader: " + name;
			tdns_log.write(msg);
			exit(1);
		}
	}

	void end() {
		Shader::active = -1;
	}
};
int Shader::active = -1;

Shader textured_shader;
Shader solid_shader;
Shader text_shader;
void init_shaders() {
	textured_shader.init("..\\..\\shaders\\textured.vs", "..\\..\\shaders\\textured.fs");
	solid_shader.init("..\\..\\shaders\\solid.vs", "..\\..\\shaders\\solid.fs");
	text_shader.init("..\\..\\shaders\\text.vs", "..\\..\\shaders\\text.fs");
}