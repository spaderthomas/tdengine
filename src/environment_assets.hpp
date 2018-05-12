void init_environment_assets() {
	Entity_Visible* test_entity = entity_table.get_entity("test_entity");
	test_entity->tilesize = glm::vec2(16, 16);
	{
		Texture* test_entity_texture = asset_table.get_texture("test_texture");
		test_entity_texture->init("../../textures/environment/building.png");
		Animation* test_animation = asset_table.get_animation("test_animation");
		test_animation->add_frame(test_entity_texture);
		test_entity->animations.push_back(test_animation);
		test_entity->start_animation("test_animation");
	}

	Entity_Visible* sand = entity_table.get_entity("sand");
	sand->tilesize = glm::vec2(1, 1);
	{
		Texture* texture = asset_table.get_texture("sand");
		texture->init("../../textures/environment/sand1.png");
		Animation* animation = asset_table.get_animation("sand");
		animation->add_frame(texture);
		sand->animations.push_back(animation);
		sand->start_animation("sand");
	}
}
