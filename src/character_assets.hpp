void init_character_assets() {
	// WILSON
	Entity_Visible* wilson = entity_table.get_entity("wilson");
	wilson->tilesize = glm::vec2(2, 2);
	// walk
	{
		// How to create an animation
		// 1: Put it in the asset table and give the animation pointer to the entity
		Animation* wilson_walk = asset_table.get_animation("wilson_walk");
		wilson->animations.push_back(wilson_walk);

		// 1: Add all the textures that comprise the frames
		Texture* wilson_walk1 = asset_table.get_texture("wilson_walk1");
		wilson_walk1->init("../../textures/wilson/wilson_walk1.png");
		Texture* wilson_walk2 = asset_table.get_texture("wilson_walk2");
		wilson_walk2->init("../../textures/wilson/wilson_walk2.png");
		Texture* wilson_walk3 = asset_table.get_texture("wilson_walk3");
		wilson_walk3->init("../../textures/wilson/wilson_walk3.png");
		Texture* wilson_walk4 = asset_table.get_texture("wilson_walk4");
		wilson_walk4->init("../../textures/wilson/wilson_walk4.png");
		Texture* wilson_walk5 = asset_table.get_texture("wilson_walk5");
		wilson_walk5->init("../../textures/wilson/wilson_walk5.png");
		// 2: Add the frames to it, in order
		wilson_walk->add_frame(wilson_walk1);
		wilson_walk->add_frame(wilson_walk2);
		wilson_walk->add_frame(wilson_walk3);
		wilson_walk->add_frame(wilson_walk1);
		wilson_walk->add_frame(wilson_walk4);
		wilson_walk->add_frame(wilson_walk5);
	}


	// BOON
	Entity_Visible* boon = entity_table.get_entity("boon");
	boon->tilesize = glm::vec2(2, 2);

	// walk
	{
		Animation* boon_walk = asset_table.get_animation("boon_walk");
		boon->animations.push_back(boon_walk);

		Texture* boon_walk1 = asset_table.get_texture("boon_walk1");
		boon_walk1->init("../../textures/boon/boon_walk1.png");
		Texture* boon_walk2 = asset_table.get_texture("boon_walk2");
		boon_walk2->init("../../textures/boon/boon_walk2.png");
		Texture* boon_walk3 = asset_table.get_texture("boon_walk3");
		boon_walk3->init("../../textures/boon/boon_walk3.png");
		Texture* boon_walk4 = asset_table.get_texture("boon_walk4");
		boon_walk4->init("../../textures/boon/boon_walk4.png");
		Texture* boon_walk5 = asset_table.get_texture("boon_walk5");
		boon_walk5->init("../../textures/boon/boon_walk5.png");

		boon_walk->add_frame(boon_walk1);
		boon_walk->add_frame(boon_walk2);
		boon_walk->add_frame(boon_walk3);
		boon_walk->add_frame(boon_walk1);
		boon_walk->add_frame(boon_walk4);
		boon_walk->add_frame(boon_walk5);
	}
}