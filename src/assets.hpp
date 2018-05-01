Texture* wall_texture;
Texture* octopus_texture;
Texture* ball_anim1;
Texture* ball_anim2;
Texture* ball_anim3;
Texture* ball_anim4;
Texture* ball_anim5;
Animation* ball_anim;
Animation* wall_anim;
Animation* octopus_anim;
Entity_Visible* wall;
Entity_Visible* octopus;
Entity_Visible* ball_ent;
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

	// TEXTURES
	wall_texture = asset_table.get_texture("wall_tex");
	octopus_texture = asset_table.get_texture("octopus_tex");
	ball_anim1 = asset_table.get_texture("ball1");
	ball_anim2 = asset_table.get_texture("ball2");
	ball_anim3 = asset_table.get_texture("ball3");
	ball_anim4 = asset_table.get_texture("ball4");
	ball_anim5 = asset_table.get_texture("ball5");
	
	wall_texture->init("../../textures/wall.jpg");
	octopus_texture->init("../../textures/octopus.png");
	ball_anim1->init("../../textures/animation_test1.png");
	ball_anim2->init("../../textures/animation_test2.png");
	ball_anim3->init("../../textures/animation_test3.png");
	ball_anim4->init("../../textures/animation_test4.png");
	ball_anim5->init("../../textures/animation_test5.png");

	
	// ANIMATIONS
	ball_anim = asset_table.get_animation("ball_anim");
	wall_anim = asset_table.get_animation("wall_anim");
	octopus_anim = asset_table.get_animation("octopus_anim");
	ball_anim->add_frame(ball_anim1);
	ball_anim->add_frame(ball_anim2);
	ball_anim->add_frame(ball_anim3);
	ball_anim->add_frame(ball_anim4);
	ball_anim->add_frame(ball_anim5);

	wall_anim->add_frame("wall_tex");

	octopus_anim->add_frame("octopus_tex");

	
	// ENTITIES
	// These will probably go on a per-level basis
	wall = entity_table.get_entity("wall");
	octopus = entity_table.get_entity("octopus");
	ball_ent = entity_table.get_entity("ball");

	ball_ent->animation_ids.push_back("ball_anim");
	octopus->animation_ids.push_back("octopus_anim");
}





