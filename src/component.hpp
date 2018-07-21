struct Component {
	virtual void save(json& j) const {
		j["kind"] = "NULL";
	};
	virtual void init_from_table(sol::table table) {};
};
struct Graphic_Component : Component {
	vector<Animation*> animations;
	Animation* active_animation = nullptr;
	glm::vec2 scale;
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

	void set_animation2(const string wish_name) {
		if (active_animation) {
			if (active_animation->name == wish_name) {
				return;
			}
		}

		set_animation(wish_name);
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

		// Set the scaling of this based on the first sprite we see. Right now, no objects resize (i.e. all sprites it could use are the same dimensions)
		// Also, use 640x360 because the raw dimensions are based on this
		Sprite* default_sprite = this->get_current_frame();
		this->scale = glm::vec2((float)default_sprite->width / (float)640, (float)default_sprite->height / (float)360);
		this->z = gc["z"];
	}
};

struct Position_Component : Component {
	glm::vec2 screen_pos;
	glm::vec2 scale;

	void init_from_table(sol::table pc) override {
		screen_pos = glm::vec2(0.f);
		scale = glm::vec2(0.f);
	}

	void save(json& j) const override {
		j["kind"] = "Position_Component";
		j["scale"]["x"] = scale.x;
		j["scale"]["y"] = scale.y;
		j["pos"]["x"] = screen_pos.x;
		j["pos"]["y"] = screen_pos.y;
	}

	void load(json& j) {
		scale.x = j["scale"]["x"];
		scale.y = j["scale"]["y"];
		screen_pos.x = j["pos"]["x"];
		screen_pos.y = j["pos"]["y"];
	}
};

struct Bounding_Box : Component {
	glm::vec2 screen_center;
	glm::vec2 screen_extents;

	void init_from_table(sol::table table) override {
		screen_center.x = table["center"]["x"];
		screen_center.y = table["center"]["y"];
		screen_extents.x = table["extents"]["x"];
		screen_extents.y = table["extents"]["y"];
	};
};

struct Movement_Component : Component {
	glm::vec2 wish;
};

// Defines a box whose bottom center is at the characters origin. Used for selecting.
struct Vision : Component {
	float width;
	float depth;

	void init_from_table(sol::table table) {
		width = table["extents"]["width"];
		depth = table["extents"]["depth"];
	}
};