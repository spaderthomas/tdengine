TableNode* Component::save() const { return nullptr; }

void Graphic_Component::begin_animation(string wish) {
	for (auto& animation : animations) {
		if (animation->name == wish) {
			active_animation = animation;
			active_animation->icur_frame = 0;
			return;
		}
	}

	string msg = "Tried to set active animation to " + wish + " but it was not registered in the component!";
	tdns_log.write(msg);
}
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
void Graphic_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Graphic Component")) {
		// Display animation info
		if (ImGui::BeginCombo("Animations", active_animation->name.c_str(), 0)) {
			for (auto anim : animations) {
				bool is_selected = anim->name == active_animation->name;
				if (ImGui::Selectable(anim->name.c_str(), is_selected)) {
					begin_animation(anim->name);
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		
		// Display scale info
		ImGui::SliderFloat2("Scale", glm::value_ptr(scale), 0.f, 1.f);
		ImGui::TreePop();
	}
}

TableNode* Position_Component::save() const {
	TableNode* self = new TableNode;
	
	// Could make something to let you create nested tables if they don't exist,
	// but typo errors + explicit > implicit means I'll leave it for now
	TableNode* pos_table = new TableNode;
	tds_set2(self, pos_table, POS_KEY);
	
	tds_set2(self, world_pos.x, POS_KEY, "x");
	tds_set2(self, world_pos.y, POS_KEY, "y");
	
	return self;
}
void Position_Component::load(TableNode* self) {
	world_pos.x = tds_float2(self, POS_KEY, "x");
	world_pos.y = tds_float2(self, POS_KEY, "y");
}
string Position_Component::name() { return "Position_Component"; }
void Position_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Position Component")) {
		ImGui::SliderFloat2("Position", glm::value_ptr(world_pos), 0.f, 2.f);
		ImGui::TreePop();
	}
}

void Movement_Component::init_from_table(TableNode* table) {
	speed.x = tds_float2(table, "speed", "x");
	speed.y = tds_float2(table, "speed", "y");
}
string Movement_Component::name() { return "Movement_Component"; }
void Movement_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Movement Component")) {
		ImGui::TreePop();
	}
}

void Vision_Component::init_from_table(TableNode* table) {
	width = tds_float2(table, "extents", "width");
	depth = tds_float2(table, "extents", "depth");
}
string Vision_Component::name() { return "Vision"; }
void Vision_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Vision Component")) {
		ImGui::TreePop();
	}
}

string Interaction_Component::name() { return "Interaction_Component"; }
void Interaction_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Interaction Component")) {
		ImGui::TreePop();
	}
}

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
void Door_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Door Component")) {
		if (ImGui::BeginCombo("##setdoor", level.c_str(), 0)) {
			for (auto& kvp : levels) {
				const string& name = kvp.first;
				bool is_selected = level.c_str() == name;
				if (ImGui::Selectable(name.c_str(), &is_selected)) {
					level = name;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::TreePop();
	}
}

string Collision_Component::name() { return "Collision_Component"; }
void Collision_Component::init_from_table(TableNode* table) {
	kind = (Collider_Kind)tds_int2(table, "kind");
	bounding_box.screen_center.x = tds_float2(table, "bounding_box", "center", "x"); 
	bounding_box.screen_center.y = tds_float2(table, "bounding_box", "center", "y");
	bounding_box.screen_extents.x = tds_float2(table, "bounding_box", "extents", "x");
	bounding_box.screen_extents.y = tds_float2(table, "bounding_box", "extents", "y");
}
void Collision_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Collision Component")) {
		ImGui::TreePop();
	}
}

string Task_Component::name() { return "Task_Component"; }
void Task_Component::init_from_table(TableNode* table) {
	this->task = new Task;
}
void Task_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Task Component")) {
		task->imgui_visualizer();
		ImGui::TreePop();
	}
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
void BattleComponent::imgui_visualizer() {
	if (ImGui::TreeNode("Battle Component")) {
		ImGui::SliderInt("Health", (int*)(&health), 0, 10);
		ImGui::TreePop();
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
void TileComponent::imgui_visualizer() {
	if (ImGui::TreeNode("Tile Component")) {
		int min = x - 20;
		int max = x + 20;
		ImGui::SliderInt("Position", &x, min, max);
		ImGui::TreePop();
	}
}
