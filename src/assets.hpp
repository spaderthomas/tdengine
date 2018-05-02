Texture* wall_texture;
Texture* octopus_texture;
Texture* ball_anim1;
Texture* ball_anim2;
Texture* ball_anim3;
Texture* ball_anim4;
Texture* ball_anim5;
Texture* boon_walk1;
Texture* boon_walk2;
Texture* boon_walk3;
Animation* ball_anim;
Animation* wall_anim;
Animation* octopus_anim;
Animation* boon_walk;
Entity_Visible* wall;
Entity_Visible* octopus;
Entity_Visible* ball_ent;
Entity_Visible* boon;
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
	wall_texture->init("../../textures/wall.jpg");
	wall_anim = asset_table.get_animation("wall_anim");
	wall_anim->add_frame("wall_tex");
	wall = entity_table.get_entity("wall");

	octopus_texture = asset_table.get_texture("octopus_tex");
	octopus_texture->init("../../textures/octopus.png");
	octopus_anim = asset_table.get_animation("octopus_anim");
	octopus_anim->add_frame("octopus_tex");
	octopus = entity_table.get_entity("octopus");
	octopus->animation_ids.push_back("octopus_anim");


	ball_anim1 = asset_table.get_texture("ball1");
	ball_anim2 = asset_table.get_texture("ball2");
	ball_anim3 = asset_table.get_texture("ball3");
	ball_anim4 = asset_table.get_texture("ball4");
	ball_anim5 = asset_table.get_texture("ball5");
	ball_anim1->init("../../textures/animation_test1.png");
	ball_anim2->init("../../textures/animation_test2.png");
	ball_anim3->init("../../textures/animation_test3.png");
	ball_anim4->init("../../textures/animation_test4.png");
	ball_anim5->init("../../textures/animation_test5.png");
	ball_anim = asset_table.get_animation("ball_anim");
	ball_anim->add_frame(ball_anim1);
	ball_anim->add_frame(ball_anim2);
	ball_anim->add_frame(ball_anim3);
	ball_anim->add_frame(ball_anim4);
	ball_anim->add_frame(ball_anim5);
	ball_ent = entity_table.get_entity("ball");
	ball_ent->animation_ids.push_back("ball_anim");

	boon = entity_table.get_entity("boon");
	// How to add an animation
	// 1: Add all the textures that comprise the frames
	boon_walk1 = asset_table.get_texture("boon_walk1");
	boon_walk1->init("../../textures/boon_walk1.png");
	boon_walk2 = asset_table.get_texture("boon_walk2");
	boon_walk2->init("../../textures/boon_walk2.png");
	boon_walk3 = asset_table.get_texture("boon_walk3");
	boon_walk3->init("../../textures/boon_walk3.png");
	// 2: Create a named animation and add the frames to it
	boon_walk = asset_table.get_animation("boon_walk");
	boon_walk->add_frame(boon_walk1);
	boon_walk->add_frame(boon_walk2);
	boon_walk->add_frame(boon_walk1);
	boon_walk->add_frame(boon_walk3);
	// 3: Give the entity you want to use this animation a handle to it
	boon->animation_ids.push_back("boon_walk");
}





