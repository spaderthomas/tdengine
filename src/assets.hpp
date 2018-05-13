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
	create_texture_atlas("../../textures/environment/");
	{
		Entity_Visible* test_entity = entity_table.get_entity("test_entity");
		test_entity->tilesize = glm::vec2(16, 16);
		{
			Sprite* test_entity_sprite = asset_table.get_sprite("building.png");
			test_entity_sprite->layer = FOREGROUND;
			Animation* test_animation = asset_table.get_animation("test_animation");
			test_animation->add_frame(test_entity_sprite);
			test_entity->animations.push_back(test_animation);
			test_entity->start_animation("test_animation");
		}

		Entity_Visible* grass = entity_table.get_entity("grass");
		grass->tilesize = glm::vec2(1, 1);
		{
			Sprite* texture = asset_table.get_sprite("grass1.png");
			texture->layer = BACKGROUND;
			Animation* animation = asset_table.get_animation("sand");
			animation->add_frame(texture);
			grass->animations.push_back(animation);
			grass->start_animation("sand");
		}
	}

	// CHARACTERS
	create_texture_atlas("../../textures/boon/");
	create_texture_atlas("../../textures/wilson/");
	{
		// WILSON
		Entity_Visible* wilson = entity_table.get_entity("wilson");
		wilson->tilesize = glm::vec2(2, 2);
		//walk
		{
			// How to create an animation
			// 1: Put it in the asset table and give the animation pointer to the entity
			Animation* wilson_walk = asset_table.get_animation("wilson_walk");
			wilson->animations.push_back(wilson_walk);

			// 1: Add all the textures that comprise the frames
			Sprite* wilson_walk1 = asset_table.get_sprite("wilson_walk1.png");
			wilson_walk1->layer = FOREGROUND;
			Sprite* wilson_walk2 = asset_table.get_sprite("wilson_walk2.png");
			wilson_walk2->layer = FOREGROUND;
			Sprite* wilson_walk3 = asset_table.get_sprite("wilson_walk3.png");
			wilson_walk3->layer = FOREGROUND;
			Sprite* wilson_walk4 = asset_table.get_sprite("wilson_walk4.png");
			wilson_walk4->layer = FOREGROUND;
			Sprite* wilson_walk5 = asset_table.get_sprite("wilson_walk5.png");
			wilson_walk5->layer = FOREGROUND;
			// 2: Add the frames to it, in order
			wilson_walk->add_frame(wilson_walk1);
			wilson_walk->add_frame(wilson_walk2);
			wilson_walk->add_frame(wilson_walk3);
			wilson_walk->add_frame(wilson_walk1);
			wilson_walk->add_frame(wilson_walk4);
			wilson_walk->add_frame(wilson_walk5);
		} 


		// BOON
		create_texture_atlas("../../textures/boon/");
		//walk
		{
			Entity_Visible* boon = entity_table.get_entity("boon");
			boon->tilesize = glm::vec2(2, 2);

			// walk
			{
				Animation* boon_walk = asset_table.get_animation("boon_walk");
				boon->animations.push_back(boon_walk);

				Sprite* boon_walk1 = asset_table.get_sprite("boon_walk1.png");
				boon_walk1->layer = FOREGROUND;
				Sprite* boon_walk2 = asset_table.get_sprite("boon_walk2.png");
				boon_walk2->layer = FOREGROUND;
				Sprite* boon_walk3 = asset_table.get_sprite("boon_walk3.png");
				boon_walk3->layer = FOREGROUND;
				Sprite* boon_walk4 = asset_table.get_sprite("boon_walk4.png");
				boon_walk4->layer = FOREGROUND;
				Sprite* boon_walk5 = asset_table.get_sprite("boon_walk5.png");
				boon_walk5->layer = FOREGROUND;

				boon_walk->add_frame(boon_walk1);
				boon_walk->add_frame(boon_walk2);
				boon_walk->add_frame(boon_walk3);
				boon_walk->add_frame(boon_walk1);
				boon_walk->add_frame(boon_walk4);
				boon_walk->add_frame(boon_walk5);
			}
		}
	}

}





