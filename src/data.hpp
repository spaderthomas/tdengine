vector<float> triangle_verts = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
}; 
vector<uint> triangle_indices = {
	0, 1, 2,
};
vector<float> triangle_tex_coords = {
    0.0f, 0.0f, 
    1.f, 0.0f,
    0.5f, 1.0f,
};

vector<float> square_verts = {
	1.f,  1.f,
	1.f, -1.f,
	-1.f, -1.f,
	-1.f,  1.f
};
glm::vec3 screen_bottom_right = glm::vec3(1.f, 1.f, 1.f);
glm::vec3 screen_top_right = glm::vec3(1.f, -1.f, 1.f);
glm::vec3 screen_top_left = glm::vec3(-1.f, -1.f, 1.f);
glm::vec3 screen_bottom_left = glm::vec3(-1.f, 1.f, 1.f);

vector<uint> square_indices = {
	0, 1, 2,
	2, 0, 3
};
vector<float> square_tex_coords = {
	1.f, 1.f,
	1.f, 0.0f,
	0.f, 0.f,
	0.f, 1.f,
};
GLvoid* square_tex_coords_offset;
vector<float> line_verts = {
	0.f, 0.f, 1.f,
	1.f, 1.f, 1.f,
};
vector<uint> line_indices = {
	0, 1,
};
