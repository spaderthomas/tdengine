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
		Background_Tile* test_entity = new Background_Tile;
		test_entity->init();
		test_entity->id = "test_entity";
		test_entity->graphic_component->tilesize = glm::vec2(16, 16);
		test_entity->graphic_component->layer = FOREGROUND;
		entity_table.add_entity(test_entity);
		{
			Sprite* test_entity_sprite = asset_table.get_sprite("building.png");
			Animation* test_animation = asset_table.get_animation("test_animation");
			test_animation->add_frame(test_entity_sprite);
			test_entity->graphic_component->animations.push_back(test_animation);
			test_entity->graphic_component->start_animation("test_animation");
		}

		Background_Tile* grass = new Background_Tile;
		grass->init();
		grass->id = "grass";
		grass->graphic_component->tilesize = glm::vec2(1, 1);
		grass->graphic_component->layer = BACKGROUND;
		entity_table.add_entity(grass);
		{
			Sprite* texture = asset_table.get_sprite("grass1.png");
			Animation* animation = asset_table.get_animation("sand");
			animation->add_frame(texture);
			grass->graphic_component->animations.push_back(animation);
			grass->graphic_component->start_animation("sand");
		}
	}

	// CHARACTERS
	create_texture_atlas("../../textures/boon/");
	create_texture_atlas("../../textures/wilson/");
	{
		// WILSON
		Player* wilson = new Player;
		wilson->init();
		wilson->id = "wilson";
		wilson->graphic_component->tilesize = glm::vec2(2, 2);
		wilson->graphic_component->layer = FOREGROUND;
		entity_table.add_entity(wilson);
		//walk
		{
			// How to create an animation
			// 1: Put it in the asset table and give the animation pointer to the entity
			Animation* wilson_walk = asset_table.get_animation("wilson_walk");
			wilson->graphic_component->animations.push_back(wilson_walk);

			// 1: Add all the textures that comprise the frames
			Sprite* wilson_walk1 = asset_table.get_sprite("wilson_walk1.png");
			Sprite* wilson_walk2 = asset_table.get_sprite("wilson_walk2.png");
			Sprite* wilson_walk3 = asset_table.get_sprite("wilson_walk3.png");
			Sprite* wilson_walk4 = asset_table.get_sprite("wilson_walk4.png");
			Sprite* wilson_walk5 = asset_table.get_sprite("wilson_walk5.png");
			// 2: Add the frames to it, in order
			wilson_walk->add_frame(wilson_walk1);
			wilson_walk->add_frame(wilson_walk2);
			wilson_walk->add_frame(wilson_walk3);
			wilson_walk->add_frame(wilson_walk1);
			wilson_walk->add_frame(wilson_walk4);
			wilson_walk->add_frame(wilson_walk5);
		} 
		wilson->graphic_component->start_animation("wilson_walk");



		// BOON
		Player* boon = new Player;
		boon->init();
		boon->id = "boon";
		boon->graphic_component->tilesize = glm::vec2(2, 2);
		boon->graphic_component->layer = FOREGROUND;
		entity_table.add_entity(boon);
		// walk
		{
			Animation* boon_walk = asset_table.get_animation("boon_walk");
			boon->graphic_component->animations.push_back(boon_walk);

			Sprite* boon_walk1 = asset_table.get_sprite("boon_walk1.png");
			Sprite* boon_walk2 = asset_table.get_sprite("boon_walk2.png");
			Sprite* boon_walk3 = asset_table.get_sprite("boon_walk3.png");
			Sprite* boon_walk4 = asset_table.get_sprite("boon_walk4.png");
			Sprite* boon_walk5 = asset_table.get_sprite("boon_walk5.png");

			boon_walk->add_frame(boon_walk1);
			boon_walk->add_frame(boon_walk2);
			boon_walk->add_frame(boon_walk3);
			boon_walk->add_frame(boon_walk1);
			boon_walk->add_frame(boon_walk4);
			boon_walk->add_frame(boon_walk5);
		}
		boon->graphic_component->start_animation("boon_walk");

	}

}





