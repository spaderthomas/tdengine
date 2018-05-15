struct Graphic_Component {
	vector<Animation*> animations;
	Animation* active_animation;
	Render_Layer layer;
	
	float time_to_next_frame; 
	float seconds_per_sprite_update = 1.f / 8.f; // How often animations move between frames
	glm::vec2 tilesize;

	glm::vec2 get_scaling() {
		return glm::vec2(tilesize.x * SCR_TILESIZE_X, tilesize.y * SCR_TILESIZE_Y);
	}

	void start_animation(string wish_anim_name) {
		// Find the animation in the entity's list 
		Animation* wish_anim = nullptr;
		fox_for(ianim, animations.size()) {
			Animation* cur_anim = animations[ianim];
			if (cur_anim->name == wish_anim_name) {
				wish_anim = cur_anim;
				break;
			}
		}

		// If we can find it, go ahead and mark it as the entity's current animation
		if (wish_anim) {
			wish_anim->reset();
			active_animation = wish_anim;
			time_to_next_frame = seconds_per_sprite_update;
		}
		else {
			string msg = "Asked to start an animation that you didn't have! Animation name was: " + wish_anim_name;
			tdns_log.write(msg.c_str());
			exit(1);
		}
	}

	bool update(float dt) {
		time_to_next_frame -= dt;
		if (time_to_next_frame <= 0.f) {
			active_animation->next_frame();
			time_to_next_frame = seconds_per_sprite_update;
		}
		return false;
	}

	void draw(SRT transform) {
		Sprite* active_sprite = active_animation->get_active_sprite();
		if (active_sprite) {
			renderer.draw_sprite(active_sprite, transform, layer);
		}
	}
	void draw(glm::ivec2 grid_pos) {
		Sprite* active_sprite = active_animation->get_active_sprite();
		SRT transform = srt_from_grid_pos(grid_pos);
		transform.scale = get_scaling();
		if (active_sprite) {
			renderer.draw_sprite(active_sprite, transform, layer);
		}
	}

};
