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
		// 0: Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), vert_offset);
		glEnableVertexAttribArray(0);
		// 1: Texture coordinates
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), tex_coord_offset);
		glEnableVertexAttribArray(1);
	}	

	void draw(GLenum mode) {
		glDrawElements(mode, (GLsizei)6, GL_UNSIGNED_INT, 0);
	}
};

GLuint Sprite::vert_buffer;
GLuint Sprite::elem_buffer;
GLuint Sprite::vao;
void bind_sprite_buffers() {
	glBindVertexArray(Sprite::vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
}

void fill_gpu_sprite_buffers() {
	vector<float> vert_data;
	vector<float> tex_coords;

	// Push each layer of square verts into the buffer, and mark offsets
	concat(vert_data, background_square_verts);
	GLvoid* foreground_offset = (GLvoid*)(sizeof(float) * vert_data.size());
	concat(vert_data, foreground_square_verts);

	// Give sprites their vertex offsets & fill tex coordinate buffer
	for (auto sprite : asset_table.sprites) {
		if (sprite->layer == BACKGROUND) { sprite->vert_offset = (GLvoid*)0; }
		else if (sprite->layer == FOREGROUND) { sprite->vert_offset = foreground_offset; }

		sprite->indices_offset = 0;
		sprite->tex_coord_offset = (GLvoid*)(sizeof(float) * vert_data.size());
		concat(vert_data, sprite->tex_coords);
	}

	// Send all the data to OpenGL buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, square_indices.size() * sizeof(uint), square_indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_data.size() * sizeof(float), vert_data.data(), GL_STATIC_DRAW);
}
