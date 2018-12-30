void Component::save(json& j) const {
	j["kind"] = "NULL";
};
void Component::init_from_table(sol::table table) {};
void Component::init_from_tdstable(TableNode* table) {};

void    Graphic_Component::add_animation(Animation* anim) {
	animations.push_back(anim);
}
Sprite* Graphic_Component::get_current_frame() {
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
void    Graphic_Component::init_from_table(sol::table gc) {
	//@leak Never free old animations if we call this twice
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

	this->z = gc["z"];
}
void Graphic_Component::init_from_tdstable(TableNode* gc) {
	TableNode* animations = tds_table(gc, "Animations");
	this->animations.clear();

	for (auto& def : animations->assignments) {
		KVPNode* kvp = (KVPNode*)def;
		Animation* animation = new Animation;
		animation->name = kvp->key;

		TableNode* frames = (TableNode*)kvp->value;
		for (uint frame_idx = 0; frame_idx < frames->assignments.size(); frame_idx++) {
			string sprite_name = tds_string(frames, to_string(frame_idx));
			Sprite* frame = asset_table.get_asset<Sprite>(sprite_name);
			animation->frames.push_back(frame);
		}

		this->add_animation(animation);
	}
}
string  Graphic_Component::name() { return "Graphic_Component"; }

void   Position_Component::save(json& j) const {
	j["kind"] = "Position_Component";
	j["scale"]["x"] = scale.x;
	j["scale"]["y"] = scale.y;
	j["pos"]["x"] = world_pos.x;
	j["pos"]["y"] = world_pos.y;
}
void Position_Component::load(json& self) {
	scale.x = self["scale"]["x"];
	scale.y = self["scale"]["y"];
	world_pos.x = self["pos"]["x"];
	world_pos.y = self["pos"]["y"];
}
string Position_Component::name() { return "Position_Component"; }

void Movement_Component::init_from_table(sol::table table) {
	speed.x = table["speed"]["x"];
	speed.y = table["speed"]["y"];
}
void Movement_Component::init_from_tdstable(TableNode* table) {
	speed.x = tds_float(table, "speed", "x");
	speed.y = tds_float(table, "speed", "y");
}
string Movement_Component::name() { return "Movement_Component"; }

void Vision_Component::init_from_table(sol::table table) {
	width = table["extents"]["width"];
	depth = table["extents"]["depth"];
}
void Vision_Component::init_from_tdstable(TableNode* table) {
	width = tds_float(table, "extents", "width");
	depth = tds_float(table, "extents", "depth");
}
string Vision_Component::name() { return "Vision"; }

void Interaction_Component::init_from_table(sol::table table) {
	other = { 0, nullptr };
	on_interact = table["on_interact"];
}
string Interaction_Component::name() { return "Interaction_Component"; }


void Door_Component::save(json& j) const {
	j["kind"] = "Door_Component";
	j["to"] = to;
	j["position.x"] = position.x;
	j["position.y"] = position.y;
}
void Door_Component::load(json& j) {
	to = j["to"];
}
string Door_Component::name() { return "Door_Component"; }

string Collision_Component::name() { return "Collision_Component"; }
void Collision_Component::init_from_table(sol::table table) {
	kind = (Collider_Kind)table["kind"];

	bounding_box.screen_center.x = table["bounding_box"]["center"]["x"];
	bounding_box.screen_center.y = table["bounding_box"]["center"]["y"];
	bounding_box.screen_extents.x = table["bounding_box"]["extents"]["x"];
	bounding_box.screen_extents.y = table["bounding_box"]["extents"]["y"];
}
void Collision_Component::init_from_tdstable(TableNode* table) {
	kind = (Collider_Kind)tds_int(table, "kind");
	bounding_box.screen_center.x = tds_float(table, "bounding_box", "center", "x"); 
	bounding_box.screen_center.y = tds_float(table, "bounding_box", "center", "y");
	bounding_box.screen_extents.x = tds_float(table, "bounding_box", "extents", "x");
	bounding_box.screen_extents.y = tds_float(table, "bounding_box", "extents", "y");
}

string Task_Component::name() { return "Task_Component"; }
void Task_Component::change_task(sol::table new_task) {
	EntityHandle actor = this->task->actor;
	this->task = new Task;
	this->task->init_from_table(new_task, actor);
}
void Task_Component::init_from_tdstable(TableNode* table) {
	EntityHandle actor = this->task->actor;
	this->task = new Task;
	this->task->init_from_tdstable(table, actor);
}
string Update_Component::name() { return "Update_Component"; }
void Update_Component::init_from_table(sol::table table) {
	update = table["update"];
}

