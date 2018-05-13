Texture_Atlas* Asset_Table::get_texture_atlas(string id) {
	// If we can find it in our current list, return it
	fox_for(iatlas, atlases.size()) {
		Texture_Atlas* cur_atlas = atlases[iatlas];
		if (cur_atlas->name == id) {
			return cur_atlas;
		}
	}

	// Otherwise, make a new one and return that
	Texture_Atlas* new_atlas = new Texture_Atlas;
	new_atlas->name = id;
	atlases.push_back(new_atlas);
	return new_atlas;
}

Sprite* Asset_Table::get_sprite(string id) {
	// If we can find it in our current list, return it
	fox_for(isprite, sprites.size()) {
		Sprite* cur_sprite = sprites[isprite];
		if (cur_sprite->name == id) {
			return cur_sprite;
		}
	}

	// Otherwise, make a new one and return that
	Sprite* new_sprite = new Sprite;
	new_sprite->name = id;
	sprites.push_back(new_sprite);
	return new_sprite;
}

Animation* Asset_Table::get_animation(string id) {
	fox_for(ianim, animations.size()) {
		Animation* cur_anim = animations[ianim];
		if (cur_anim->name == id) {
			return cur_anim;
		}
	}

	Animation* new_animation = new Animation;
	new_animation->name = id;
	animations.push_back(new_animation);
	return new_animation;
}

Mesh* Asset_Table::get_mesh(string id) {
	fox_for(imesh, meshes.size()) {
		Mesh* cur_mesh = meshes[imesh];
		if (cur_mesh->name == id) {
			return cur_mesh;
		}
	}

	Mesh* new_mesh = new Mesh;
	new_mesh->name = id;
	meshes.push_back(new_mesh);
	return new_mesh;
}