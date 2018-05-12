Shader textured_shader;
Shader solid_shader;
Mesh* line;
Mesh* square;

void init_assets() {
	// SHADERS
	textured_shader.init("../../shaders/textured.vs", "../../shaders/textured.fs");
	solid_shader.init("../../shaders/solid.vs", "../../shaders/solid.fs");

	// MESHES
	line = asset_table.get_mesh("line");
	line->verts = line_verts;
	line->indices = line_indices;
	square = asset_table.get_mesh("square");
	square->verts = square_verts;
	square->indices = square_indices;

	// ENVIRONMENT
	init_environment_assets();
	init_character_assets();
}





