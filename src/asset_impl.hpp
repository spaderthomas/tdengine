template <typename Asset_Type>
void AssetManager::add_asset(std::string name, Asset_Type* asset) {
	static_assert(std::is_base_of_v<Asset, Asset_Type>, "To add an asset to the asset table, its type must derive from Asset.");
	if (assets.find(name) != assets.end()) {
		tdns_log.write("Tried to register asset with name " + name + ", but it already existed.", Log_Flags::File);
	}

	asset->name = name;
	assets[name] = asset;

	tdns_log.write("Registered asset: " + name, Log_Flags::File);
}

template <typename Asset_Type>
Asset_Type* AssetManager::get_asset(std::string name) const {
	auto it = assets.find(name);
	if (it == assets.end()) return nullptr;

	Asset* asset = it->second;
	Asset_Type* typed_asset = dynamic_cast<Asset_Type*>(asset);
	if (typed_asset) return typed_asset;

	tdns_log.write("Tried to get asset of name " + name + ". Found it, but requested it as the wrong type.");
	return nullptr;
}

template <typename Asset_Type>
std::vector<Asset_Type*> AssetManager::get_all() {
	std::vector<Asset_Type*> all;
	for (auto& [name, asset] : assets) {
		Asset_Type* asset_as_type = dynamic_cast<Asset_Type*>(asset);
		if (asset_as_type) {
			all.push_back(asset_as_type);
		}
	}
	
	return all;
}


void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, handle);
}


void destroy_sprite(std::string name) {
	auto& asset_manager = get_asset_manager();
	auto sprite = asset_manager.get_asset<Sprite>(name);
	if (!sprite) return;

	asset_manager.assets.erase(name);
	free(sprite);
}

void load_other_textures(const char* directory) {
	char** texture_files = physfs_enumerate(directory);
	defer { physfs_free(texture_files); };
	
	for (char** texture_file = texture_files; *texture_file; ++texture_file) {
		tdns_log.write("@asset:load::" + std::string(*texture_file), Log_Flags::File);
		
		add_single_sprite(*texture_file);

		// file_watcher.watch(path.string(), [path = path]() {
		// 	tdns_log.write("@asset:update: " + path.string(), Log_Flags::File);
		// 	if (!dumb_is_valid_png(path)) return;
			
		// 	destroy_texture(path);
		// 	add_single_sprite(path);
			
		// 	auto& asset_manager = get_asset_manager();
		// 	auto sprite = asset_manager.get_asset<Sprite>(path.stem().string());
		// 	sprite->tex_coord_offset = square_tex_coords_offset;
		// });
	}
};

void add_single_sprite(const char* texture_file) {
	auto& asset_manager = get_asset_manager();

	auto texture = create_texture(texture_file);
	if (!texture) return;

	Sprite* sprite = new Sprite;
	sprite->name = strip_extension(texture_file);
	sprite->texture = texture;
	sprite->height = texture->height;
	sprite->width = texture->width;
	sprite->num_channels = texture->num_channels;
	sprite->tex_coords = {
		1, 1,
		1, 0,
		0, 0,
		0, 1
	};
	
	asset_manager.add_asset<Sprite>(sprite->name, sprite);
}

Texture* create_texture(const char* texture_name) {
	Texture* texture = new Texture;

	// Load the file's data
	auto handle = PHYSFS_openRead(texture_name);
	auto len = PHYSFS_fileLength(handle);
	unsigned char* buffer = (unsigned char*)calloc(sizeof(char), len);
	PHYSFS_readBytes(handle, buffer, len);
	defer { free(buffer); PHYSFS_close(handle); };

	// Feed it to STBI
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load_from_memory(buffer, len, &texture->width, &texture->height, &texture->num_channels, 0);
	
	// Create all the OpenGL internals and point it to the newly created texture
	glGenTextures(1, &texture->handle);
	//glActiveTexture(GL_TEXTURE0); // Note: not all graphics drivers have default 0!
	glBindTexture(GL_TEXTURE_2D, texture->handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Give the texture to the asset manager
	auto& asset_manager = get_asset_manager();
	asset_manager.add_asset<Texture>(texture_name, texture);
	return asset_manager.get_asset<Texture>(texture_name);
}

void destroy_texture(std::filesystem::path path) {
	if (!path.has_filename()) return;
	if (path.extension() != ".png") return;

	auto& asset_manager = get_asset_manager();
	auto texture = asset_manager.get_asset<Texture>(path.filename().string());
	if (!texture) return;

	auto sprites = asset_manager.get_all<Sprite>();
	for (auto sprite : sprites) {
		if (sprite->texture == texture) {
			destroy_sprite(sprite->name);
		}
	}

	glDeleteTextures(1, &texture->handle);
	asset_manager.assets.erase(path.filename().string());
	free(texture);
}

void reload_texture_atlas() {
	create_texture_atlas();
	load_texture_coordinates();
}

/*
void init_sprites() {
	// These sprites get their own texture -- no atlas.
	std::vector<AbsolutePath> ignore_atlas_directories = {
		AbsolutePath(RelativePath("asset/art/backgrounds")),
		AbsolutePath(RelativePath("asset/art/transitions")),
	};

	// You can have a startup option to load ALL textures like this. This lets
	// you hotload textures without the hiccup of building a whole atlas.
	if (g_texture_mode == TextureMode::Atlas) {
		std::string dir = absolute_path("asset/art");
		create_texture_atlas(dir);
	}
	else if (g_texture_mode == TextureMode::AtlasWithHotloading) {
		std::string dir = absolute_path("asset/art");
		create_texture_atlas(dir);
	}
	else {
		auto art_path = AbsolutePath(RelativePath("asset/art"));
		ignore_atlas_directories.push_back(art_path);
	}

	for (auto& directory : ignore_atlas_directories) {
		auto as_fs_path = directory.path;
		load_sprites_from_directory(as_fs_path);
	}
}

void create_texture_atlas(std::string assets_dir) {
	tdns_log.write("Creating an atlas from files at " + assets_dir, Log_Flags::File);
	
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

	// Go through each sprite, register it in the asset table, and collect its rect data
	int rect_id = 0;
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
*/

void bind_sprite_buffers() {
	glBindVertexArray(Sprite::vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
}

bool Sprite::is_initialized() const{
	return (nullptr != this->texture);
}

void Mesh::bind() {
	glBindVertexArray(Mesh::vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh::elem_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Mesh::vert_buffer);

	// 0: Vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), vert_offset);
	glDisableVertexAttribArray(1);
}

void Mesh::draw(GLuint mode) {
	glDrawElements(mode, (GLsizei)indices.size(), GL_UNSIGNED_INT, indices_offset);
}

void init_mesh() {
	auto& manager = get_asset_manager();
	
	Mesh* line = new Mesh;
	line->verts = line_verts;
	line->indices = line_indices;
	manager.add_asset<Mesh>("line", line);

	Mesh* square = new Mesh;
	square->verts = square_verts;
	square->indices = square_indices;
	manager.add_asset<Mesh>("square", square);
}

void Animation::add_frames(std::vector<std::string>& frames_to_add) {
	for (auto& frame : frames_to_add) {
		this->add_frame(frame);
	}
}

void Animation::add_frame(std::string& frame) {
	auto& asset_manager = get_asset_manager();
	Sprite* sprite = asset_manager.get_asset<Sprite>(frame);
	if (!sprite) {
		tdns_log.write("Tried to add a frame to an animation, but couldn't find the sprite.");
		tdns_log.write("Requested sprite: " + frame);
		tdns_log.write("Animation name" + this->name);
		return;
	}

	frames.push_back(frame);
}

std::string Animation::get_frame(int frame) {
	return this->frames[frame];
}
	

AssetManager& get_asset_manager() {
	static AssetManager manager;
	return manager;
}


void create_texture_atlas() {
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
		for (auto data : image_data) { (data); }
	};

	// Go through each sprite, register it in the asset table, and collect its rect data
	int rect_id = 0;
	char** texture_files = physfs_enumerate("textures");
	for (char** texture_file = texture_files; *texture_file; ++texture_file) {
		auto handle = PHYSFS_openRead(*texture_file);
		auto len = PHYSFS_fileLength(handle);
		unsigned char* buffer = (unsigned char*)calloc(sizeof(char), len);
		
		PHYSFS_readBytes(handle, buffer, len);
		image_data.push_back(buffer);
		PHYSFS_close(handle);
		
		auto sprite_name = strip_extension(*texture_file);

		// This makes this function idempotent
		destroy_sprite(sprite_name);

		// Add the sprite to the asset manager
		Sprite* sprite = new Sprite;
		sprite->texture = atlas;
		sprite->name = sprite_name;

		asset_manager.add_asset<Sprite>(sprite->name, sprite);
						
		// Load the image data, create a rectangle for it
		stbi_load_from_memory(buffer, len, &sprite->width, &sprite->height, &sprite->num_channels, 0);
			
		stbrp_rect rect;
		rect.id = rect_id++;
		rect.w = sprite->width;
		rect.h = sprite->height;
		rects.push_back(rect);
			
		Name_And_ID new_id = { sprite->name, rect.id };
		sprite_ids.push_back(new_id);
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
	
	load_other_textures("textures/backgrounds");
	load_other_textures("textures/transitions");
}
