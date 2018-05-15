struct Mesh : Asset {
	vector<float> verts;
	vector<uint> indices;
	vector<float> tex_coords;
	
	GLvoid* vert_offset;
	GLvoid* indices_offset;
	GLvoid* tex_coord_offset;
	bool use_tex_coords = false;

	static GLuint vert_buffer;
	static GLuint elem_buffer;
	static GLuint vao;

	void bind() {
		glBindVertexArray(Mesh::vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh::elem_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, Mesh::vert_buffer);

		// 0: Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), vert_offset);
		glEnableVertexAttribArray(0);
		if (use_tex_coords) {
			// 1: Texture coordinates
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), tex_coord_offset);
			glEnableVertexAttribArray(1);
		}
	}

	void draw(GLuint mode) {
		glDrawElements(mode, (GLsizei)indices.size(), GL_UNSIGNED_INT, indices_offset);
	}
};
GLuint Mesh::vert_buffer;
GLuint Mesh::elem_buffer;
GLuint Mesh::vao;


// Fill all_meshes with all game meshes, then call this
void fill_gpu_mesh_buffers() {
	vector<Mesh*>& all_meshes = asset_table.meshes;
	vector<float> vert_buffer;
	vector<uint> indx_buffer;
	
	// Collect all vertices and indices
	fox_for(imesh, all_meshes.size()) {
		Mesh* mesh = all_meshes[imesh];
		mesh->indices_offset = (GLvoid*)(sizeof(uint) * indx_buffer.size());
		indx_buffer.insert(indx_buffer.end(), mesh->indices.begin(), mesh->indices.end());
		mesh->vert_offset = (GLvoid*)(sizeof(float) * vert_buffer.size());
		vert_buffer.insert(vert_buffer.end(), mesh->verts.begin(), mesh->verts.end());
	}
	// Collect all tex coords
	fox_for(imesh, all_meshes.size()) {
		Mesh* mesh = all_meshes[imesh];
		mesh->tex_coord_offset = (GLvoid*)(sizeof(float) * vert_buffer.size());
		vert_buffer.insert(vert_buffer.end(), mesh->tex_coords.begin(), mesh->tex_coords.end());
	}
	
	// Give them to the OpenGL buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indx_buffer.size() * sizeof(uint), indx_buffer.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, Mesh::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_buffer.size() * sizeof(float), vert_buffer.data(), GL_STATIC_DRAW);
}
