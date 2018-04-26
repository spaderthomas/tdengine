struct Sprite {
	static GLuint vert_buffer;
	static GLuint elem_buffer;
	static GLuint vao;

	static vector<float> square_verts;
	static vector<uint> square_indices; 
	static vector<float> square_tex_coords;

	Texture texture;
	//vector<Texture> animation;

	unsigned char* texture_data;
	
	void bind() {
		glBindVertexArray(Sprite::vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);

		// 0: Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		// 1: Texture coordinates
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)(sizeof(float) * square_verts.size()));
		glEnableVertexAttribArray(1);

		// Use the correct texture
		texture.bind();
	}
	
	void draw(GLuint mode) {
		glDrawElements(mode, (GLsizei)square_indices.size(), GL_UNSIGNED_INT, 0);
	}

};
GLuint Sprite::vert_buffer;
GLuint Sprite::elem_buffer;
GLuint Sprite::vao;
vector<float> Sprite::square_verts = {
	1.f,  1.f, 0.0f,
	1.f, -1.f, 0.0f,
	-1.f, -1.f, 0.0f,
	-1.f,  1.f, 0.0f 
};
vector<uint> Sprite::square_indices = {
	0, 1, 2,
	2, 0, 3
};
vector<float> Sprite::square_tex_coords = {
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
	concat(vert_buffer, Sprite::square_verts);
	concat(vert_buffer, Sprite::square_tex_coords);
	concat(indx_buffer, Sprite::square_indices);
	
	// Give them to the OpenGL buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indx_buffer.size() * sizeof(uint), indx_buffer.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_buffer.size() * sizeof(float), vert_buffer.data(), GL_STATIC_DRAW);
	gl_unbind_buffer();	
}
