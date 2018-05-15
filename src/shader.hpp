struct Shader {
	unsigned int id;
	
	void check_gl_error(GLint shader, GLint kind) {
		int success;
		char info_log[512] = {0};
		glGetShaderiv(shader, kind, &success);
		if(!success) {
			glGetShaderInfoLog(shader, 512, NULL, info_log);
			tdns_log.write(info_log);
			exit(1);
		}
	}
	
	void init(const char* vs_path, const char* fs_path) {
		const char* paths[] = {
			vs_path,
			fs_path
		};

		unsigned int shader_program;
		shader_program = glCreateProgram();
		
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
			glShaderSource(shader, 1, &source, NULL);
            glCompileShader(shader);
            check_gl_error(shader, GL_COMPILE_STATUS);
            glAttachShader(shader_program, shader);
		}
		
		// Link into a shader program
		glLinkProgram(shader_program);
		check_gl_error(shader_program, GL_LINK_STATUS);
		id = shader_program;
	}

	unsigned int get_uniform_loc(const char* name) {
		return glGetUniformLocation(id, name);
	}

	void set_vec4(const char* name, glm::vec4& vec) {
		glUniform4f(get_uniform_loc(name), vec.x, vec.y, vec.z, vec.w);
	}
	void set_vec3(const char* name, glm::vec3& vec) {
		glUniform3f(get_uniform_loc(name), vec.x, vec.y, vec.z);
	}
	void set_mat3(const char* name, glm::mat3& mat) {
		glUniformMatrix3fv(get_uniform_loc(name), 1, GL_FALSE, glm::value_ptr(mat));
	}
	void set_int(const char* name, GLint val) {
		glUniform1i(get_uniform_loc(name), val);
	}

	void bind() {
		glUseProgram(id);
	}
};
