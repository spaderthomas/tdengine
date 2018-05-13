enum Layer { BACKGROUND, FOREGROUND };

struct Sprite : Asset {
	int height, width, num_channels;
	Texture_Atlas* atlas;
	vector<float> tex_coords;
	Layer layer;

	GLvoid* vert_offset;
	GLvoid* indices_offset;
	GLvoid* tex_coord_offset;

	static GLuint vert_buffer;
	static GLuint elem_buffer;
	static GLuint vao;


	void bind() {
		glBindVertexArray(Sprite::vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);

		// 0: Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), vert_offset);
		glEnableVertexAttribArray(0);
		// 1: Texture coordinates
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), tex_coord_offset);
		glEnableVertexAttribArray(1);
	}	

	void draw(GLenum mode) {
		int x = 3;
		glDrawElements(mode, (GLsizei)6, GL_UNSIGNED_INT, 0);
	}
};
GLuint Sprite::vert_buffer;
GLuint Sprite::elem_buffer;
GLuint Sprite::vao;

vector<float> background_square_verts = {
	1.f,  1.f, 0.0f,
	1.f, -1.f, 0.0f,
	-1.f, -1.f, 0.0f,
	-1.f,  1.f, 0.0f
};
vector<float> foreground_square_verts = {
	1.f,  1.f, 0.f,
	1.f, -1.f, 0.f,
	-1.f, -1.f, 0.f,
	-1.f,  1.f, 0.f
};
void fill_gpu_sprite_buffers() {
	vector<float> verts;
	vector<float> tex_coords;
	vector<uint> indices;

	// Push each layer of square verts into the buffer, and mark offsets
	concat(verts, background_square_verts);
	GLvoid* foreground_offset = (GLvoid*)(sizeof(float) * verts.size());
	concat(verts, foreground_square_verts);

	// Generate offsets for sprites & fill tex coordinate buffer
	for (auto sprite : asset_table.sprites) {
		if (sprite->layer == BACKGROUND) { sprite->vert_offset = (GLvoid*)0; }
		//else if (sprite->layer == FOREGROUND) { sprite->vert_offset = foreground_offset; }
		else if (sprite->layer == FOREGROUND) { sprite->vert_offset = (GLvoid*)0; }

		sprite->indices_offset = 0;
		sprite->tex_coord_offset = (GLvoid*)(sizeof(float) * verts.size());
		concat(verts, sprite->tex_coords);
	}

	// Add indices
	concat(indices, square_indices);

	// Send all the data to OpenGL buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
}
