struct Entity_Visible {
	// We're only painting squares, so this is the data for the square that all Entities use
	string id;
	vector<Animation*> animations;
	Animation* active_animation;
	float time_to_next_frame; 
	float seconds_per_sprite_update = 1.f / 8.f; // How often animations move between frames
	glm::vec2 tilesize;

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
			cout << "Asked to start an animation that you didn't have!\n";
			cout << "Animation name was: " << wish_anim_name;
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

	void bind() {
		// If the animation we're using isn't valid, just return
		if (!active_animation) { 
			string msg = "There is no active animation set on " + id + ", but bind() was called";
			cout << msg;
			tdns_log.write(msg.c_str());
			return;
		}

		// Use the correct texture
		active_animation->bind();
	}
	
	void draw(GLuint mode) {
		glDrawElements(mode, (GLsizei)6, GL_UNSIGNED_INT, 0);
	}

};
