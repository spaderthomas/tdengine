void create_texture_atlases() {
	auto& asset_manager = get_asset_manager();
	
	Texture* atlas = new Texture;
	atlas->name = "atlas";

	// Create some memory for our atlas
	stbi_set_flip_vertically_on_load(false);
	int count_atlas_bytes = sizeof(int32) * REGULAR_ATLAS_SIZE * REGULAR_ATLAS_SIZE;
	int32* atlas_data = (int32*)malloc(count_atlas_bytes);
	defer { free(atlas_data); };
	memset(atlas_data, 0x0, count_atlas_bytes);

	// Init some data for stb_rectpack
	std::vector<stbrp_rect> rects;
	std::vector<Name_And_ID> sprite_ids;
	std::vector<unsigned char*> image_data;
	defer {
		for (auto stbi_data : image_data) { stbi_image_free(stbi_data); }
	};

	auto handle = PHYSFS_openRead("textures/fuck.txt");
	for (char** texture_name = texture_names; *texture_name; ++texture_name) {
		printf("texture name: %s\n", *texture_name);
	}	

	// Go through each sprite, register it in the asset table, and collect its rect data
	char** texture_names = physfs_enumerate("textures");
	int rect_id = 0;
	for (char** texture_name = texture_names; *texture_name; ++texture_name) {
		
	for (directory_iterator iter(assets_dir); iter != directory_iterator(); ++iter) {
		const auto& path = iter->path();

		// Guard against the subdirectories + ensure only PNGs
		if (!path.has_filename()) continue;
		if (path.extension() != ".png") continue;

		if (g_texture_mode == TextureMode::AtlasWithHotloading) {
			file_watcher.watch(path.string(), &reload_texture_atlas);
		}

		auto sprite_name = path.stem().string();

		// This makes this function idempotent
		destroy_sprite(sprite_name);

		// Add the sprite to the asset manager
		Sprite* sprite = new Sprite;
		sprite->texture = atlas;
		sprite->name = sprite_name;

		asset_manager.add_asset<Sprite>(sprite->name, sprite);
						
		// Load the image data, create a rectangle for it
		unsigned char* data = stbi_load(path.string().c_str(), &sprite->width, &sprite->height, &sprite->num_channels, 0);
		if (data) {
			image_data.push_back(data);
			
			stbrp_rect rect;
			rect.id = rect_id++;
			rect.w = sprite->width;
			rect.h = sprite->height;
			rects.push_back(rect);
			
			Name_And_ID new_id = { sprite->name, rect.id };
			sprite_ids.push_back(new_id);
		}
	}

	// Pack the rectangles
	stbrp_context* context = (stbrp_context*)calloc(1, sizeof(stbrp_context));
	defer { free(context); };
	stbrp_node* nodes = (stbrp_node*)calloc(REGULAR_ATLAS_SIZE, sizeof(stbrp_node));
	defer { free(nodes); };
	stbrp_init_target(context, REGULAR_ATLAS_SIZE, REGULAR_ATLAS_SIZE, nodes, REGULAR_ATLAS_SIZE);
	stbrp_pack_rects(context, rects.data(), rects.size());

	// Write them to the image buffer and mark offsets in the sprites
	fox_for(irect, rects.size()) {
		auto& rect = rects[irect];
		for (auto& name_and_id : sprite_ids) {
			if (rect.id == name_and_id.id) {
				Sprite* sprite = asset_manager.get_asset<Sprite>(name_and_id.name);
				// top right
				sprite->tex_coords.push_back((rect.x + rect.w) / (float)REGULAR_ATLAS_SIZE);
				sprite->tex_coords.push_back((rect.y) / (float)REGULAR_ATLAS_SIZE);
				// bottom right
				sprite->tex_coords.push_back((rect.x + rect.w) / (float)REGULAR_ATLAS_SIZE);
				sprite->tex_coords.push_back((rect.y + rect.h) / (float)REGULAR_ATLAS_SIZE);
				// bottom left
				sprite->tex_coords.push_back((rect.x) / (float)REGULAR_ATLAS_SIZE);
				sprite->tex_coords.push_back((rect.y + rect.h) / (float)REGULAR_ATLAS_SIZE);
				// top left
				sprite->tex_coords.push_back((rect.x) / (float)REGULAR_ATLAS_SIZE);
				sprite->tex_coords.push_back((rect.y) / (float)REGULAR_ATLAS_SIZE);
				break;
			}
		}

		// Copy the sprite into the image buffer
		int32* image_cur_row = (int32*)image_data[irect];
		int32* atlas_cur_row = atlas_data + rect.x + REGULAR_ATLAS_SIZE * rect.y;

		fox_for(row, rect.h) {
			memcpy(atlas_cur_row, image_cur_row, rect.w * sizeof(int32));
			atlas_cur_row += REGULAR_ATLAS_SIZE;
			image_cur_row += rect.w;
		}
	}

	// Write the atlas itself, with the same name as the folder it was created from	
	std::string atlas_path = absolute_path("build/atlas.png");
	stbi_write_png(atlas_path.c_str(), REGULAR_ATLAS_SIZE, REGULAR_ATLAS_SIZE, 4, atlas_data, 0);

	// Now, create all the OpenGL internals and point it to the newly created atlas
	glGenTextures(1, &atlas->handle);
	glActiveTexture(GL_TEXTURE0); // Note: not all graphics drivers have default 0!
	glBindTexture(GL_TEXTURE_2D, atlas->handle);

	// Some sane defaults
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Give it the image data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, REGULAR_ATLAS_SIZE, REGULAR_ATLAS_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	atlas->width = REGULAR_ATLAS_SIZE;
	atlas->height = REGULAR_ATLAS_SIZE;
	atlas->num_channels = 4;

	asset_manager.add_asset<Texture>(atlas->name, atlas);
}

void init_sprites() {
	create_texture_atlas();

	for (auto& directory : ignore_atlas_directories) {
		auto as_fs_path = directory.path;
		load_sprites_from_directory(as_fs_path);
	}
}
