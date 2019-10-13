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

void Graphic_Component::init(TableNode* table) {
	this->z = tds_int2(table, "z");
	TableNode* animations = tds_table2(table, "Animations");
	this->animations.clear();
	
	for (auto& def : animations->assignments) {
		KVPNode* kvp = (KVPNode*)def;
		Animation* animation = new Animation;
		animation->name = kvp->key;
		
		animation->add_frames((TableNode*)kvp->value);
		
		this->add_animation(animation);
	}

	this->begin_animation(tds_string2(table, DEFAULT_ANIMATION_KEY));
	
	// Set the scaling of this based on the first sprite we see. Right now, no objects resize
	// Also, use 640x360 because the raw dimensions are based on this
	Sprite* default_sprite = this->get_current_frame();
	if (default_sprite) {
		scale = glm::vec2((float)default_sprite->width / (float)640,
						  (float)default_sprite->height / (float)360);
	} else {
		scale = glm::vec2(1.f, 1.f);

		// @spader 10/11/2019: Wish we had some context as to who this GC belonged to so as to have a helpful error message.
		string msg = "[GRAPHIC COMPONENT INIT]\n";
		msg += "Tried to initialize dimensions using its default sprite, but there wasn't one. Used (1.f, 1.f) instead.";
		tdns_log.write(msg);
	}

}
string Graphic_Component::name() { return "Graphic_Component"; }
void Graphic_Component::imgui_visualizer() {
	if (ImGui::TreeNode("Graphic Component")) {
		defer {ImGui::TreePop(); };
		if (ImGui::TreeNode("Animations")) {
			defer {ImGui::TreePop(); };

			if (!this->active_animation && !this->animations.empty()) {
				this->active_animation = this->animations[0];
			}
			
			if (active_animation) {
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
				
					for (auto frame : active_animation->frames) {
						ImGui::Text(frame->name.c_str());
					}
				}
			}

			// If the user presses a button, show them a modal where they can make the new GC
			if (ImGui::Button("Add New Animation")) {
				ImGui::OpenPopup("New Animation");
			}

			static char name_buf[256];
			static char frame_buf[256];
			static vector<string> frames;
			static auto close_modal = []() {
				memset(name_buf, 0, 256);
				memset(frame_buf, 0, 256);
				ImGui::CloseCurrentPopup();
			};
			
			if (ImGui::BeginPopupModal("New Animation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				defer { ImGui::EndPopup(); };
				ImGui::InputText("Name", name_buf, 256);
				ImGui::InputText("##new_frame", frame_buf, 256);
				ImGui::SameLine();
				if (ImGui::Button("Add Frame")) {
					frames.push_back(frame_buf);
					memset(frame_buf, 0, 256);
				}

				for (auto& frame : frames) {
					ImGui::Text(frame.c_str());
				}

				// Cancel: Throw everything away
				ImGui::PushStyleColor(ImGuiCol_Button, ImGuiColor_Red);
				if (ImGui::Button("Cancel")) {
					close_modal();
				}
				ImGui::PopStyleColor();

				ImGui::SameLine();

				// Add: Add the animation to the component
				ImGui::PushStyleColor(ImGuiCol_Button, ImGuiColor_Green);
				if (ImGui::Button("Add")) {
					auto animation = new Animation;
					animation->name = name_buf;
					animation->add_frames(frames);
					this->animations.push_back(animation);
					close_modal();
				}
				ImGui::PopStyleColor();
			}			
		}
		
		// Display scale info
		ImGui::SliderFloat2("Scale", glm::value_ptr(scale), 0.f, 1.f);
	}
}

TableNode* Graphic_Component::make_template() const {
	TableNode* self = new TableNode;

	tds_set2(self, z, Z_KEY);
	
	if (animations.size()) {
		tds_set2(self, animations[0]->name, DEFAULT_ANIMATION_KEY);
	} else {
		tds_set2(self, "", DEFAULT_ANIMATION_KEY);
	}

	TableNode* animations_table = new TableNode;
	tds_set2(self, animations_table, ANIMATIONS_KEY);

	return self;
}
