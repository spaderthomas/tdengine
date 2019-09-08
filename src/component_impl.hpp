TableNode* Component::save() const { return nullptr; }

void Graphic_Component::add_animation(Animation* anim) {
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
	this->z = tds_int2(gc, "z");
	TableNode* animations = tds_table2(gc, "Animations");
	this->animations.clear();
	
	for (auto& def : animations->assignments) {
		KVPNode* kvp = (KVPNode*)def;
		Animation* animation = new Animation;
		animation->name = kvp->key;
		
		TableNode* frames = (TableNode*)kvp->value;
		for (uint frame_idx = 0; frame_idx < frames->assignments.size(); frame_idx++) {
			string sprite_name = tds_string2(frames, to_string(frame_idx));
			Sprite* frame = asset_table.get_asset<Sprite>(sprite_name);
			animation->frames.push_back(frame);
		}
		
		this->add_animation(animation);
	}
}
string Graphic_Component::name() { return "Graphic_Component"; }

TableNode* Position_Component::save() const {
	TableNode* self = new TableNode;
	
	// Could make something to let you create nested tables if they don't exist,
	// but typo errors + explicit > implicit means I'll leave it for now
	TableNode* scale_table = new TableNode;
	tds_set2(self, scale_table, SCALE_KEY);
	TableNode* pos_table = new TableNode;
	tds_set2(self, pos_table, POS_KEY);
	
	tds_set2(self, scale.x, SCALE_KEY, "x");
	tds_set2(self, scale.y, SCALE_KEY, "y");
	tds_set2(self, world_pos.x, POS_KEY, "x");
	tds_set2(self, world_pos.y, POS_KEY, "y");
	
	return self;
}
void Position_Component::load(TableNode* self) {
	scale.x = tds_float2(self, SCALE_KEY, "x");
	scale.y = tds_float2(self, SCALE_KEY, "y");
	world_pos.x = tds_float2(self, POS_KEY, "x");
	world_pos.y = tds_float2(self, POS_KEY, "y");
}
string Position_Component::name() { return "Position_Component"; }

void Movement_Component::init_from_table(TableNode* table) {
	speed.x = tds_float2(table, "speed", "x");
	speed.y = tds_float2(table, "speed", "y");
}
string Movement_Component::name() { return "Movement_Component"; }

void Vision_Component::init_from_table(TableNode* table) {
	width = tds_float2(table, "extents", "width");
	depth = tds_float2(table, "extents", "depth");
}
string Vision_Component::name() { return "Vision"; }

string Interaction_Component::name() { return "Interaction_Component"; }

void Door_Component::init_from_table(TableNode* table) {
	level = tds_string2(table, LEVEL_KEY);
}
TableNode* Door_Component::save() const {
	TableNode* self = new TableNode;
	tds_set2(self, level, LEVEL_KEY);
	
	return self;
}
void Door_Component::load(TableNode* self) {
	level = tds_string2(self, LEVEL_KEY);
}
string Door_Component::name() { return "Door_Component"; }

string Collision_Component::name() { return "Collision_Component"; }
void Collision_Component::init_from_table(TableNode* table) {
	kind = (Collider_Kind)tds_int2(table, "kind");
	bounding_box.screen_center.x = tds_float2(table, "bounding_box", "center", "x"); 
	bounding_box.screen_center.y = tds_float2(table, "bounding_box", "center", "y");
	bounding_box.screen_extents.x = tds_float2(table, "bounding_box", "extents", "x");
	bounding_box.screen_extents.y = tds_float2(table, "bounding_box", "extents", "y");
}

string Task_Component::name() { return "Task_Component"; }
void Task_Component::init_from_table(TableNode* table) {
	this->task = new Task;
}

string BattleComponent::name() { return "BattleComponent"; }
void BattleComponent::init_from_table(TableNode* table) {
	this->health = tds_int2(table, "health");

	// Put the pointer to the move data that we parsed in at init time
	// into the component's list of moves
	TableNode* moves_table = tds_table2(table, "moves");
	for (int idx = 0; idx < moves_table->assignments.size(); idx++) {
		string move_name = tds_string2(moves_table, to_string(idx));
		moves.push_back(move_data[move_name]);
	}
}

string TileComponent::name() { return "TileComponent"; }
TableNode* TileComponent::save() const {
	TableNode* self = new TableNode;

	tds_set2(self, x, "x");
	tds_set2(self, y, "y");

	return self;
}
void TileComponent::load(TableNode* self) {
	this->x = tds_int2(self, "x");
	this->y = tds_int2(self, "y");
}
