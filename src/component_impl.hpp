void Component::save(json& j) const {
	j["kind"] = "NULL";
};
void Component::init_from_table(TableNode* table) {};

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
void Graphic_Component::init_from_table(TableNode* gc) {
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

void Movement_Component::init_from_table(TableNode* table) {
	speed.x = tds_float(table, "speed", "x");
	speed.y = tds_float(table, "speed", "y");
}
string Movement_Component::name() { return "Movement_Component"; }

void Vision_Component::init_from_table(TableNode* table) {
	width = tds_float(table, "extents", "width");
	depth = tds_float(table, "extents", "depth");
}
string Vision_Component::name() { return "Vision"; }

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
void Collision_Component::init_from_table(TableNode* table) {
	kind = (Collider_Kind)tds_int(table, "kind");
	bounding_box.screen_center.x = tds_float(table, "bounding_box", "center", "x"); 
	bounding_box.screen_center.y = tds_float(table, "bounding_box", "center", "y");
	bounding_box.screen_extents.x = tds_float(table, "bounding_box", "extents", "x");
	bounding_box.screen_extents.y = tds_float(table, "bounding_box", "extents", "y");
}

string Task_Component::name() { return "Task_Component"; }
void Task_Component::init_from_table(TableNode* table) {
	//@hack: This is so that we don't have to specify the owning entity every time,
	//       but surely there is a better way to do this
	if (this->task) 
		EntityHandle actor = this->task->actor;
	this->task = new Task;
}

