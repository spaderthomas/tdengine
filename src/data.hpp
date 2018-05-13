// In-program shapes
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

// @todo: Merge these with the Entity_Visible version?
vector<float> square_verts = {
	1.f,  1.f, 0.0f,
	1.f, -1.f, 0.0f,
	-1.f, -1.f, 0.0f,
	-1.f,  1.f, 0.0f
};
vector<float> background_square_verts = {
	1.f,  1.f, 0.0f,
	1.f, -1.f, 0.0f,
	-1.f, -1.f, 0.0f,
	-1.f,  1.f, 0.0f
};
vector<float> foreground_square_verts = {
	1.f,  1.f, -1.f,
	1.f, -1.f, -1.f,
	-1.f, -1.f, -1.f,
	-1.f,  1.f, -1.f
};

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
vector<float> line_verts = {
	0.f, 0.f, 0.f,
	1.f, 1.f, 0.f,
};
vector<uint> line_indices = {
	0, 1,
};

