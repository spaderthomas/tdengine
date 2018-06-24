namespace stdfs = std::experimental::filesystem;

string atlas_folders[] = {"../../textures/boon/",
						  "../../textures/wilson/",	
						  "../../textures/environment/",	
						 };

#define REGULAR_ATLAS_SIZE 1024

struct Texture_Atlas : Asset {
	GLuint handle;

	int width, height, num_channels;

	void bind() {
		glBindTexture(GL_TEXTURE_2D, handle);
	}
};

// We need this struct so we can identify which rectangle goes to which sprite when we pack them
struct Name_And_ID {
	string name;
	int id;
};
void create_texture_atlas(string relative_dir) {
	// Create some memory for our atlas
	stbi_set_flip_vertically_on_load(false);
	int32* atlas_data = (int32*)malloc(REGULAR_ATLAS_SIZE * REGULAR_ATLAS_SIZE * sizeof(int32));
	memset(atlas_data, 0x0, sizeof(int32) * REGULAR_ATLAS_SIZE * REGULAR_ATLAS_SIZE);
	
	// Extract name of the atlas png from the dir name
	string atlas_name;
	for (int ichar = relative_dir.size() - 1; ichar > -1; ichar--) { // Start at -2 to throw away trailing /
		if (relative_dir.at(ichar) == '/') { break; }
		atlas_name.insert(atlas_name.begin(), relative_dir.at(ichar));
	}
	atlas_name += ".png";
	Texture_Atlas* atlas = asset_table.get_asset<Texture_Atlas>(atlas_name);

	const stdfs::directory_iterator end{};
	vector<stbrp_rect> rects;
	vector<Name_And_ID> sprite_ids;
	vector<unsigned char*> image_data;

	// Go through each sprite, register it in the asset table, and collect its rect data
	int rect_id = 0;
	for(stdfs::directory_iterator iter{relative_dir} ; iter != end ; ++iter ) {
		string path = iter->path().string();

		// Make sure it's actually a PNG (probably a magic number for this, but meh)
		if (!(path.at(path.size() - 3) == 'p' && path.at(path.size() - 2) == 'n' && path.at(path.size() - 1) == 'g')) { continue; }

		// Extract the actual name of the .png from the path, put it in the asset table
		string asset_name;
		for (int ichar = path.size() - 1; ichar > -1; ichar--) {
			if (path.at(ichar) == '\\') { break; }
			asset_name.insert(asset_name.begin(), path.at(ichar));
		}
		Sprite* sprite = asset_table.get_asset<Sprite>(asset_name);
		sprite->atlas = atlas;

		// Load the image data, create a rectangle for it
		unsigned char* data = stbi_load(path.c_str(), &sprite->width, &sprite->height, &sprite->num_channels, 0);
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

	// Pack the rectangles
	stbrp_context* context = (stbrp_context*)calloc(1, sizeof(stbrp_context));
	stbrp_node* nodes = (stbrp_node*)calloc(REGULAR_ATLAS_SIZE, sizeof(stbrp_node));
	stbrp_init_target(context, REGULAR_ATLAS_SIZE, REGULAR_ATLAS_SIZE, nodes, REGULAR_ATLAS_SIZE);
	stbrp_pack_rects(context, rects.data(), rects.size());

	// Write them to the image buffer and mark offsets in the sprites
	fox_for(irect, rects.size()) {
		auto& rect = rects[irect];
		for (auto& name_and_id : sprite_ids) {
			if (rect.id == name_and_id.id) {
				Sprite* sprite = asset_table.get_asset<Sprite>(name_and_id.name);
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
	
	string atlas_path = "../../textures/atlases/" + atlas->name;
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
	free(atlas_data);
}
