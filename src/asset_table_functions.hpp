Texture* Asset_Table::get_texture(string id) {
	// If we can find it in our current list, return it
	fox_for(itex, textures.size()) {
		Texture* cur_tex = textures[itex];
		if (cur_tex->name == id) {
			return cur_tex;
		}
	}

	// Otherwise, make a new one and return that
	Texture* new_tex = new Texture;
	new_tex->name = id;
	textures.push_back(new_tex);
	return new_tex;
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