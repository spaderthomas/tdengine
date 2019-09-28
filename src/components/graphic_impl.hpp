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
