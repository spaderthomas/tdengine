struct Entity_Visible {
	// We're only painting squares, so this is the data for the square that all Entities use
	static GLuint vert_buffer;
	static GLuint elem_buffer;
	static GLuint vao;

	static vector<float> square_verts;
	static vector<uint> square_indices; 
	static vector<float> square_tex_coords;

	string id;
	vector<string> animation_ids;
	int icur_anim = -1;
	float time_to_next_frame;
	float seconds_per_sprite_update = 1.f / 8.f;

	void start_animation(string anim_name) {
		// If the name is a valid animation for this entity, set our index to point to it and reset the animation's frame
		auto iter = find(animation_ids.begin(), animation_ids.end(), anim_name);
		if (iter != animation_ids.end()) {
			icur_anim = iter - animation_ids.begin();
			Animation* cur_anim = asset_table.get_animation(anim_name);
			cur_anim->reset();
			time_to_next_frame = seconds_per_sprite_update;
		}
		else {
			cout << "Asked to start an animation that you didn't have!\n";
			cout << "Animation name was: " << anim_name;
			tdns_log.write("Asked to start an animation that you didn't have! Animation name was: ");
			tdns_log.write(anim_name.c_str());
			exit(1);
		}
	}

	bool update(float dt) {
		time_to_next_frame -= dt;
		if (time_to_next_frame <= 0.f) {
			Animation* active_animation = asset_table.get_animation(animation_ids[icur_anim]);
			active_animation->next_frame();
			time_to_next_frame = seconds_per_sprite_update;
		}
		return false;
	}

	void bind() {
		// If the animation we're using isn't valid, just return
		if (icur_anim < 0 || icur_anim >= (int)animation_ids.size()) { 
			cout << "Failed to load texture!";
			tdns_log.write("Failed to load texture!");
			return;
		}

		// Otherwise, bind all the OpenGL stuff
		glBindVertexArray(Entity_Visible::vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Entity_Visible::elem_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, Entity_Visible::vert_buffer);

		// 0: Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		// 1: Texture coordinates
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)(sizeof(float) * square_verts.size()));
		glEnableVertexAttribArray(1);

		// Use the correct texture
		Animation* active_animation = asset_table.get_animation(animation_ids[icur_anim]);
		active_animation->bind();
	}
	
	void draw(GLuint mode) {
		glDrawElements(mode, (GLsizei)square_indices.size(), GL_UNSIGNED_INT, 0);
	}

};
GLuint Entity_Visible::vert_buffer;
GLuint Entity_Visible::elem_buffer;
GLuint Entity_Visible::vao;
vector<float> Entity_Visible::square_verts = {
	1.f,  1.f, 0.0f,
	1.f, -1.f, 0.0f,
	-1.f, -1.f, 0.0f,
	-1.f,  1.f, 0.0f
};
vector<uint> Entity_Visible::square_indices = {
	0, 1, 2,
	2, 0, 3
};
vector<float> Entity_Visible::square_tex_coords = {
	1.f, 1.f,
	1.f, 0.0f,
	0.f, 0.f,
	0.f, 1.f,
};

// Just gives OpenGL the data needed to draw a texture-mapped square
void fill_gpu_sprite_buffers() {
	vector<float> vert_buffer;
	vector<uint> indx_buffer;

	// Vertex buffer gets verts and indices for a square
	concat(vert_buffer, Entity_Visible::square_verts);
	concat(vert_buffer, Entity_Visible::square_tex_coords);
	concat(indx_buffer, Entity_Visible::square_indices);
	
	// Give them to the OpenGL buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Entity_Visible::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indx_buffer.size() * sizeof(uint), indx_buffer.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, Entity_Visible::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_buffer.size() * sizeof(float), vert_buffer.data(), GL_STATIC_DRAW);
	gl_unbind_buffer();	
}
