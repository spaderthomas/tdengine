template <typename Asset_Type>
void AssetManager::add_asset(std::string name, Asset_Type* asset) {
	static_assert(std::is_base_of_v<Asset, Asset_Type>, "To add an asset to the asset table, its type must derive from Asset.");
	if (assets.find(name) != assets.end()) {
		tdns_log.write("Tried to register asset with name " + name + ", but it already existed.");
	}

	asset->name = name;
	assets[name] = asset;
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

//@leak never free stbi memory
// @spader This is only used for the text box....kill it!
void create_texture(std::string path) {
	std::string texture_name = name_from_full_path(path);
	if (is_valid_filename(texture_name)) {
		Texture* new_texture = new Texture;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(absolute_path(path).c_str(), &new_texture->width, &new_texture->height, &new_texture->num_channels, 0);
		if (data) {
			defer { free(data); };
			// Now, create all the OpenGL internals and point it to the newly created atlas
			glGenTextures(1, &new_texture->handle);
			glActiveTexture(GL_TEXTURE0); // Note: not all graphics drivers have default 0!
			glBindTexture(GL_TEXTURE_2D, new_texture->handle);

			// Some sane defaults
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, new_texture->width, new_texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			auto& asset_manager = get_asset_manager();
			asset_manager.add_asset<Texture>(texture_name, new_texture);
		} else {
			std::string msg = "stb_image failed to load an image. Path was: " + path;
			tdns_log.write(msg);
		}
	}
}

void create_texture_atlas(std::string assets_dir) {
	auto& asset_manager = get_asset_manager();
	
	// Extract name of the atlas png from the dir name (e.g. /environment will output an atlas to /atlases/environment.png)
	std::string atlas_name = name_from_full_path(assets_dir);
	if (is_alphanumeric(atlas_name)) {
		atlas_name += ".png";
		Texture* atlas = new Texture;

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

		// Go through each sprite, register it in the asset table, and collect its rect data
		int rect_id = 0;
		auto recursive_add_from_dir = [&](std::string root_dir) -> void{
			// The meat; given a directory, recursively add all PNGs
			auto lambda = [&](std::string dir, const auto& lambda) -> void {
				for (directory_iterator iter(dir); iter != directory_iterator(); ++iter) {
					std::string asset_path = iter->path().string();

					// Recurse to find assets in subdirectories
					if (is_directory(iter->status())) { 
						lambda(asset_path, lambda);
					}
					// If it's a regular file, check that it's a PNG and if so, load it
					else if (is_regular_file(iter->status())) {
						if (!is_png(asset_path)) { continue; }

						std::string asset_name = name_from_full_path(asset_path);
						Sprite* sprite = new Sprite;
						sprite->atlas = atlas;
						sprite->name = asset_name;

						auto& asset_manager = get_asset_manager();
						asset_manager.add_asset<Sprite>(asset_name, sprite);
						
						// Load the image data, create a rectangle for it
						unsigned char* data = stbi_load(asset_path.c_str(), &sprite->width, &sprite->height, &sprite->num_channels, 0);
						if (data) {
							image_data.push_back(data);
							stbrp_rect new_rect;
							new_rect.id = rect_id++;
							new_rect.w = sprite->width;
							new_rect.h = sprite->height;
							rects.push_back(new_rect);
							Name_And_ID new_id = { asset_name, new_rect.id };
							sprite_ids.push_back(new_id);
						}
					}
				}
			};

			// Kick off the recursion
			lambda(root_dir, lambda);
		};
		recursive_add_from_dir(assets_dir);

		// Pack the rectangles
		stbrp_context* context = (stbrp_context*)calloc(1, sizeof(stbrp_context)); defer { free(context); };
		stbrp_node* nodes = (stbrp_node*)calloc(REGULAR_ATLAS_SIZE, sizeof(stbrp_node)); defer { free(nodes); };
		stbrp_init_target(context, REGULAR_ATLAS_SIZE, REGULAR_ATLAS_SIZE, nodes, REGULAR_ATLAS_SIZE);
		stbrp_pack_rects(context, rects.data(), rects.size());

		// Write them to the image buffer and mark offsets in the sprites
		fox_for(irect, rects.size()) {
			auto& rect = rects[irect];
			for (auto& name_and_id : sprite_ids) {
				if (rect.id == name_and_id.id) {
					Sprite* sprite = asset_manager.get_asset<Sprite>(name_and_id.name);
					// top right
					sprite->tex_coords.push_back((rect.x + rect.w) / 1024.f);
					sprite->tex_coords.push_back((rect.y) / 1024.f);
					// bottom right
					sprite->tex_coords.push_back((rect.x + rect.w) / 1024.f);
					sprite->tex_coords.push_back((rect.y + rect.h) / 1024.f);
					// bottom left
					sprite->tex_coords.push_back((rect.x) / 1024.f);
					sprite->tex_coords.push_back((rect.y + rect.h) / 1024.f);
					// top left
					sprite->tex_coords.push_back((rect.x) / 1024.f);
					sprite->tex_coords.push_back((rect.y) / 1024.f);
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
		std::string atlas_path = absolute_path("textures/atlases/") + atlas->name;
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

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, REGULAR_ATLAS_SIZE, REGULAR_ATLAS_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Give it the image data
		atlas->width = REGULAR_ATLAS_SIZE;
		atlas->height = REGULAR_ATLAS_SIZE;
		atlas->num_channels = 4;

		asset_manager.add_asset<Texture>(atlas_name, atlas);
	} else {
		std::string msg = "Invalid texture atlas name. Expected alpanumeric, but got: " + atlas_name;
		tdns_log.write(msg);
	}
}

void create_all_texture_atlas() {
	std::string atlas_dirs[] = {
		g_paths.character_texture_path,
		g_paths.other_texture_path,
		g_paths.tile_texture_path
	};

	for (auto& dir : atlas_dirs) {
		create_texture_atlas(dir);
	}
}

void bind_sprite_buffers() {
	glBindVertexArray(Sprite::vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
}

bool Sprite::is_initialized() const{
	return (nullptr != this->atlas);
}

void Mesh::bind() {
	glBindVertexArray(Mesh::vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh::elem_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Mesh::vert_buffer);

	// 0: Vertices
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), vert_offset);
	glEnableVertexAttribArray(0);
	if (use_tex_coords) {
		// 1: Texture coordinates
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), tex_coord_offset);
		glEnableVertexAttribArray(1);
	}
	else {
		glDisableVertexAttribArray(1);
	}
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

