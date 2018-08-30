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
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), vert_offset);
		glEnableVertexAttribArray(0);
		if (use_tex_coords) {
			// 1: Texture coordinates
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), tex_coord_offset);
			glEnableVertexAttribArray(1);
		}
		else {
			glDisableVertexAttribArray(1);
		}
	}

	void draw(GLuint mode) {
		glDrawElements(mode, (GLsizei)indices.size(), GL_UNSIGNED_INT, indices_offset);
	}
};
GLuint Mesh::vert_buffer;
GLuint Mesh::elem_buffer;
GLuint Mesh::vao;

Mesh* line;
Mesh* square;

void init_mesh() {
	line = asset_table.get_asset<Mesh>("line");
	line->verts = line_verts;
	line->indices = line_indices;
	square = asset_table.get_asset<Mesh>("square");
	square->verts = square_verts;
	square->indices = square_indices;
}

// Fill all_meshes with all game meshes, then call this
void fill_gpu_mesh_buffers() {

}
