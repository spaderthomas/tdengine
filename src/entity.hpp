struct Entity {
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

	void start_animation(string anim_name) {
		// If the name is a valid animation for this entity, set our index to point to it and reset the animation's frame
		auto iter = find(animation_ids.begin(), animation_ids.end(), anim_name);
		if (iter != animation_ids.end()) {
			icur_anim = iter - animation_ids.begin();
			Animation* cur_anim = asset_table.get_animation(anim_name);
			cur_anim->reset();
		}
	}

	void bind() {
		// If the animation we're using isn't valid, just return
		if (icur_anim < 0 || icur_anim >= animation_ids.size()) { 
			cout << "Failed to load texture!";
			tdns_log.write("Failed to load texture!");
			return;
		}

		// Otherwise, bind all the OpenGL stuff
		glBindVertexArray(Entity::vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Entity::elem_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, Entity::vert_buffer);

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
		Animation* active_animation = asset_table.get_animation(animation_ids[icur_anim]);
		active_animation->next_frame();
	}

};
GLuint Entity::vert_buffer;
GLuint Entity::elem_buffer;
GLuint Entity::vao;
vector<float> Entity::square_verts = {
	1.f,  1.f, 0.0f,
	1.f, -1.f, 0.0f,
	-1.f, -1.f, 0.0f,
	-1.f,  1.f, 0.0f 
};
vector<uint> Entity::square_indices = {
	0, 1, 2,
	2, 0, 3
};
vector<float> Entity::square_tex_coords = {
	1.f, 1.f, 
	1.f, 0.0f,
	0.f, 0.f,
	0.f, 1.f,
};
vector<float> square_verts_temp = {
	.5f,  .5f, 0.0f,
	.5f, -.5f, 0.0f,
	-.5f, -.5f, 0.0f,
	-.5f,  .5f, 0.0f 
};


// Just gives OpenGL the data needed to draw a texture-mapped square
void fill_gpu_sprite_buffers() {
	vector<float> vert_buffer;
	vector<uint> indx_buffer;

	// Vertex buffer gets verts and indices for a square
	concat(vert_buffer, Entity::square_verts);
	concat(vert_buffer, Entity::square_tex_coords);
	concat(indx_buffer, Entity::square_indices);
	
	// Give them to the OpenGL buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Entity::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indx_buffer.size() * sizeof(uint), indx_buffer.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, Entity::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_buffer.size() * sizeof(float), vert_buffer.data(), GL_STATIC_DRAW);
	gl_unbind_buffer();	
}
