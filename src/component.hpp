struct Component {
	virtual void save(json& j) const {
		j["kind"] = "NULL";
	};
	virtual void init_from_table(sol::table table) {};
};
struct Graphic_Component : Component {
	vector<Animation*> animations;
	Animation* active_animation = nullptr;
	int z;

	void set_animation(const string wish_name) {
		for (auto& animation : animations) {
			if (animation->name == wish_name) {
				active_animation = animation;
				active_animation->icur_frame = 0;
				return;
			}
		}

		string msg = "Tried to set active animation to " + wish_name + " but it was not registered in the component!";
		tdns_log.write(msg);
	}

	void add_animation(Animation* anim) {
		animations.push_back(anim);
	}

	Sprite* get_current_frame() {
		if (!active_animation) {
			tdns_log.write("Component asked for current frame but no active animation was set!");
			return (Sprite*)nullptr;
		}
		if (active_animation->icur_frame == -1) {
			string msg = "Active animation (" + active_animation->name + ") had invalid current frame";
			tdns_log.write(msg);
			return (Sprite*)nullptr;
		}

		return active_animation->frames[active_animation->icur_frame];
	}

	//@leak Never free old animations if we call this twice
	void init_from_table(sol::table gc) override {
		sol::table animations = gc["Animations"];
		this->animations.clear();

		// Loop through the mappings from names of animations to lists of frames
		animations.for_each([this](sol::object const& name, sol::object const& frames) {
			Animation* new_anim = new Animation;
			new_anim->name = name.as<string>();

			// Add each frame to the animation
			frames.as<sol::table>().for_each([&new_anim](sol::object const& key, sol::object const& value) {
				string frame_name = value.as<string>();
				Sprite* frame = asset_table.get_asset<Sprite>(frame_name);
				new_anim->frames.push_back(frame);
			});

			// Add the animation to the Graphic_Component's vector of animations
			this->add_animation(new_anim);
		});

		sol::object default_animation_name = gc["default_animation"];
		set_animation(default_animation_name.as<string>());

		this->z = gc["z"];
	}
};

struct Position_Component : Component {
	SRT transform;

	void init_from_table(sol::table pc) override {
		transform.translate.x = pc["translate"]["x"];
		transform.translate.y = pc["translate"]["y"];
		transform.translate.z = pc["translate"]["z"];
	
		//@hack I feel like there's a way to auto-detect this
		int px_size_x = pc["scale"]["x"];
		int px_size_y = pc["scale"]["y"];
		transform.scale.x = (px_size_x / 16) * SCR_TILESIZE_X;
		transform.scale.y = (px_size_y / 16) * SCR_TILESIZE_Y;

		transform.rad_rot = pc["rad_rot"];
	}

	void save(json& j) const override {
		j["kind"] = "Position_Component";
		transform.save(j["transform"]);
	}

	void load(json& j) {
		transform.load(j["transform"]);
	}
};

struct Collision_Component : Component {
	float top;
	float bottom;
	float left;
	float right;

	void init_from_table(sol::table table) override {
		top = table["top"];
		bottom = table["bottom"];
		left = table["left"];
		right = table["right"];
	};
};