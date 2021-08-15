void Shader::init(std::string vs_path, std::string fs_path, std::string name) {
	normalize_path(vs_path);
	normalize_path(fs_path);
	std::string paths[] = {
		vs_path,
		fs_path
	};

	int success;

	unsigned int shader_program;
	shader_program = glCreateProgram();
	
	fox_for(ishader, 2) {
		// Read in shader data
		std::string path = paths[ishader];
		FILE *shader_source_file = fopen(path.c_str(), "rb");
		if (!shader_source_file) {
			tdns_log.write("@invalid_shader_file: " + path);
		}

		fseek(shader_source_file, 0, SEEK_END);
		unsigned int fsize = ftell(shader_source_file);
		fseek(shader_source_file, 0, SEEK_SET);

		// @leak
		char *source = (char*)calloc(fsize + 1, sizeof(char));
		fread(source, fsize, 1, shader_source_file);
		source[fsize] = 0;
		fclose(shader_source_file);

		// Compile the shader
		unsigned int shader_kind = (ishader == 0) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
		unsigned int shader = glCreateShader(shader_kind);

		glShaderSource(shader, 1, &source, NULL);

		free(source);

		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, compilation_status);
			std::string message = "@shader_compile_error: ";
			tdns_log.write(message + compilation_status);
		}
		glAttachShader(shader_program, shader);
	}
		
	// Link into a shader program
	glLinkProgram(shader_program);
	glGetShaderiv(shader_program, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader_program, 512, NULL, compilation_status);
		std::string message = "@shader_compile_error: ";
		tdns_log.write(message + compilation_status);
	}

	// Push the data into the shader. If anything fails, the shader won't get
	// the new GL handles
	id = shader_program;
	glGetProgramiv(shader_program, GL_ACTIVE_UNIFORMS, (int*)&num_uniforms);
	
	this->name = name;
	this->vs_path = vs_path;
	this->fs_path = fs_path;
}

unsigned int Shader::get_uniform_loc(const char* name) {
	// Shader failed to load properly. Don't spam the log.
	if (num_uniforms == 0) return -1;
	
	auto loc = glGetUniformLocation(id, name);
	if (loc == -1) {
		tdns_log.write("@no_such_uniform: " + std::string(name) + ", " + std::string(this->name));
	}
	return loc;
}

void Shader::set_vec4(const char* name, glm::vec4 vec) {
	glUniform4f(get_uniform_loc(name), vec.x, vec.y, vec.z, vec.w);
	if (!tdns_find(uniforms_set_this_call, name)) {
		uniforms_set_this_call.push_back(name);
	}
}
void Shader::set_vec3(const char* name, glm::vec3 vec) {
	glUniform3f(get_uniform_loc(name), vec.x, vec.y, vec.z);
	if (!tdns_find(uniforms_set_this_call, name)) {
		uniforms_set_this_call.push_back(name);
	}
}
void Shader::set_vec2(const char* name, glm::vec2 vec) {
	glUniform2f(get_uniform_loc(name), vec.x, vec.y);
	if (!tdns_find(uniforms_set_this_call, name)) {
		uniforms_set_this_call.push_back(name);
	}
}
void Shader::set_mat3(const char* name, glm::mat3 mat) {
	glUniformMatrix3fv(get_uniform_loc(name), 1, GL_FALSE, glm::value_ptr(mat));
	if (!tdns_find(uniforms_set_this_call, name)) {
		uniforms_set_this_call.push_back(name);
	}
}
void Shader::set_mat4(const char* name, glm::mat4 mat) {
	glUniformMatrix4fv(get_uniform_loc(name), 1, GL_FALSE, glm::value_ptr(mat));
	if (!tdns_find(uniforms_set_this_call, name)) {
		uniforms_set_this_call.push_back(name);
	}
}
void Shader::set_int(const char* name, GLint val) {
	glUniform1i(get_uniform_loc(name), val);
	if (!tdns_find(uniforms_set_this_call, name)) {
		uniforms_set_this_call.push_back(name);
	}
}
void Shader::set_float(const char* name, GLfloat val) {
	glUniform1f(get_uniform_loc(name), val);
	if (!tdns_find(uniforms_set_this_call, name)) {
		uniforms_set_this_call.push_back(name);
	}
}

void Shader::begin() {
	if (Shader::active != -1) {
		tdns_log.write("Called begin() with shader: " + name + ", but there was already an active shader. Did you forget to call end()?");
		exit(1);
	}
	glUseProgram(id);
	Shader::active = id;
	uniforms_set_this_call.clear();
}

void Shader::check() {
	if (uniforms_set_this_call.size() != num_uniforms) {
		std::string msg = "You didn't fill in all of the uniforms! Shader was: " + std::string(name);
		tdns_log.write(msg);
		exit(1);
	}
	if (Shader::active != (int)id) {
		std::string msg = "Checked shader before draw, but it was not set. Did you forget to call begin()? Shader: " + name;
		tdns_log.write(msg);
		exit(1);
	}
}

void Shader::end() {
	Shader::active = -1;
}

void init_shaders() {
	auto& shaders = get_shader_manager();
	shaders.add(
        absolute_path("asset/shaders/textured.vs"),
	    absolute_path("asset/shaders/textured.fs"),
	    "textured");
	
	shaders.add(
	    absolute_path("asset/shaders/textured.vs"),
		absolute_path("asset/shaders/highlighted.fs"),
		"highlighted");
	
	shaders.add(
		absolute_path("asset/shaders/solid.vs"),
		absolute_path("asset/shaders/solid.fs"),
		"solid");
	
	shaders.add(
        absolute_path("asset/shaders/textured.vs"),
		absolute_path("asset/shaders/text.fs"),
		"text");
	
	shaders.add(
		absolute_path("asset/shaders/fade.vs"),
		absolute_path("asset/shaders/fade.fs"),
		"fade");
	
	shaders.add(
		absolute_path("asset/shaders/battle_transition.vs"),
		absolute_path("asset/shaders/battle_transition.fs"),
		"battle_transition");
}

void ShaderManager::add(std::string vs_path, std::string fs_path, std::string name) {
	Shader shader;
	shader.init(vs_path, fs_path, name);
	shaders[name] = shader;

	file_watcher.watch(vs_path, [this, vs_path = vs_path](){
		for (auto& [name, shader] : shaders) {
			if (are_strings_equal(shader.vs_path, vs_path)) {
				std::cout << "reloading shader: " << name << std::endl;
				shader.init(shader.vs_path, shader.fs_path, shader.name);
			}
		}
	});
	
	file_watcher.watch(fs_path, [this, fs_path = fs_path](){
		for (auto& [name, shader] : shaders) {
			if (are_strings_equal(shader.fs_path, fs_path)) {
				std::cout << "reloading shader: " << name << std::endl;
				shader.init(shader.vs_path, shader.fs_path, shader.name);
			}
		}
	});
}

Shader* ShaderManager::get(const char* name) {
	auto it = shaders.find(name);
	if (it == shaders.end()) {
		tdns_log.write(std::string("@missing_shader: ") + name);
		return nullptr;
	}

	return &(it->second);
}

ShaderManager& get_shader_manager() {
	static ShaderManager manager;
	return manager;
}
