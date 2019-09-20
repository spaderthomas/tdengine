
void Particle::make_alive() {
	velocity = glm::vec2(0.f);
	color = brown;
	y_baseline = rand_float(1.f);
	position = glm::vec2(0.f, y_baseline);
	y_offset.amp = rand_float(.5f);
	life = 1.f;
	alive = true;
}
void Particle_System::init() {
	free_list.data = (int*)malloc(1024 * sizeof(int));
	free_list.capacity = 1024;
}
void Particle_System::start() {
	fox_for(i, 1024) {
		free_list.push_back(i);
		particles[i].alive = false;
	}
}
void Particle_System::update(float dt) {
	// Emit 2 particles per frame
	fox_for(ipart, 2) {
		auto index = free_list.pop_front();
		if (index) {
			particles[*index].make_alive();
		}
	}
	
	fox_iter(it, particles) {
		auto& particle = *it;
		if (particle.alive) {
			Center_Box box = { particle.position, particle.scale };
			glm::vec4 energycolor = glm::vec4(particle.color.x, particle.color.y, particle.color.z, particle.life);
			draw_square(box, energycolor);
			
			particle.life -= dt;
			particle.position.x += dt;
			particle.position.y = particle.y_baseline + particle.y_offset.eval_at(particle.life);
			
			if (particle.life <= 0) {
				particle.alive = false;
				int index = it - particles.begin();
				free_list.push_back(index);
			}
		}
	}
}

void Battle::init() {
	battlers[0] = Entity::create("boi");
	battlers[1] = Entity::create("boi");
	
	fox_for (ibattler, 2) {
		auto pc = battlers[0]->get_component<Position_Component>();
		pc->world_pos = glm::vec2((ibattler ? 1.0f : -1.0f) * 0.25f, 0.0f);
	}
}

void Battle::render() {
	fox_for (ibattler, 2) {
		draw_entity(battlers[ibattler], Render_Flags::None);
	}
	
	renderer.render_for_frame();
}

void Battle::update(float dt) {
	if (show_console) {
		console.Draw("tdconsole");
	}
	
	camera.following = battlers[0];
	if (input.is_down[GLFW_KEY_W]) {
		camera.offset += glm::vec2{0, .025};
	}
	if (input.is_down[GLFW_KEY_S]) {
		camera.offset += glm::vec2{0, -.025};
	}
	if (input.is_down[GLFW_KEY_A]) {
		camera.offset += glm::vec2{-.025, 0};
	}
	if (input.is_down[GLFW_KEY_D]) {
		camera.offset += glm::vec2{.025, 0};
	}

	ImGui::Begin("Battle View!", 0, ImGuiWindowFlags_AlwaysAutoResize);
	int unique_btn_index = 0;
	fox_for(idx, 2) {
		EntityHandle actor = battlers[idx];
		auto bc = actor->get_component<BattleComponent>();
		ImGui::Text("Player %d", idx);
		ImGui::Text("Health: %d", bc->health);
		
		ImGui::Text("Moves:");
		for (auto move : bc->moves) {
			string unique_move_id = move->name + to_string(unique_btn_index);
			unique_btn_index++;
			if (ImGui::Button(unique_move_id.c_str())) {
				EntityHandle other = idx ? battlers[0] : battlers[1];
				auto other_bc = other->get_component<BattleComponent>();
				other_bc->health -= move->power;
			}
		}
	}
	ImGui::End();
	
}



void Cutscene::init(string name, TableNode* table) {
	this->name = name;
	this->level = levels[tds_string2(table, LEVEL_KEY)];

	TableNode* actions = tds_table2(table, ACTIONS_KEY);
	for (KVPNode* kvp : actions->assignments) {
		TableNode* action_table = (TableNode*)kvp->value;

		// Some actions have no attached entity.
		string entity_name = "";
		if (action_table->has_key(ENTITY_KEY)) {
			entity_name = tds_string2(action_table, ENTITY_KEY);
		}

		EntityHandle entity = this->level->get_first_matching_entity(entity_name);
		Action* action = action_from_table(action_table, entity);
		this->task.add_action(action);
	}
}

void Cutscene::update(float dt) {
	frame++;
	task.update(dt);
}

void Cutscene_Thing::update(float dt) {
	if (show_console) {
		console.Draw("tdconsole");
	}

	// @spader 8/22/2019: This is kind of hacky. I mean, we always want this to happen, but do I want
	// the camera to know what layer it's camera-ing for?
	camera.update(dt);
	input.world_pos = input.screen_pos + camera.offset;
	
	if (active_cutscene && !active_cutscene->done) {
		active_cutscene->update(dt);
	}
	
	physics_system.process(1.f / 60.f);
}

void Cutscene_Thing::render() {
	if (active_cutscene)
		active_cutscene->level->draw();
	
	renderer.render_for_frame();
	text_box.render();
}

void Cutscene_Thing::init() {
	for (auto& [name, cutscene] : cutscenes) {
		free(cutscene);
	}
	this->active_cutscene = nullptr;
	
	TableNode* all_cutscenes = tds_table(CUTSCENES_KEY);
	for (auto node : all_cutscenes->assignments) {
		KVPNode* kvp = (KVPNode*)node;
		Cutscene* cutscene = new Cutscene;
		cutscene->init(kvp->key, (TableNode*)kvp->value);
		this->cutscenes[kvp->key] = cutscene;
	}
}

void Cutscene_Thing::reload() {
	// Save the old state 
	string old_active_cutscene = active_cutscene->name;
	
	// Do this so no old state hangs around from the last time the cutscene ran
	active_cutscene->level->clear_entities();
	camera.following = NULL_ENTITY;
	
	// Reload the scripts for the cutscene and reload them into C++ structs 
	ScriptManager.script_dir(absolute_path(path_join({"src", "scripts", "cutscenes"})));
	init();
	// Restore state
	do_cutscene(old_active_cutscene);
}
bool Cutscene_Thing::does_cutscene_exist(const string& which) {
	for (auto& [name, cutscene] : cutscenes) {
		if (name == which) return true;
	}

	return false;
}

void Cutscene_Thing::do_cutscene(string which) {
	auto cutscene = cutscenes[which];
	this->active_level = cutscene->level;
	this->active_level->replace_entities(tds_table(CUTSCENES_KEY, which, ENTITIES_KEY));

	auto cutscene_actions = tds_table(CUTSCENES_KEY, which, ACTIONS_KEY);
	fox_for(i, cutscene->task.action_queue.size()) {
		TableNode* action_table = tds_table2(cutscene_actions, to_string(i));
		string entity_name = "";
		if (action_table->has_key(ENTITY_KEY)) {
			entity_name = tds_string2(action_table, ENTITY_KEY);
		}

		Action* action = cutscene->task.action_queue[i];
		action->actor = active_level->get_first_matching_entity(entity_name);
	}
	
	this->active_cutscene = cutscene;
	this->camera.offset = { 0.f, 0.f };
}

void Cutscene_Thing::exec_console_cmd(char* command_line) {
	char* command = strtok(command_line, " ");
	if (!command) return;
	
	if (console.Stricmp(command, "cutscene") == 0) {
		char* which = strtok(NULL, " ");
		if (does_cutscene_exist())
			do_cutscene(which);
	} else if (console.Stricmp(command, "reload") == 0) {
		reload();
	} else {
		console.AddLog("Unknown command: '%s'. Loading up Celery Man instead.\n", command_line);
	}

}

// 
void Cutscene_Thing::exit() {
	if (this->active_level)
		this->active_level->load();
}
	
void Game::init() {
	particle_system.init();
	active_dialogue = new Dialogue_Tree;
	active_level = levels["overworld"]; //@hack Probably a better way to do this
	camera.following = g_hero;
}
void Game::update(float dt) {
	static int frame = 0;
	
	if (show_console) {
		console.Draw("tdconsole");
	}
	
	// Set up the camera so that the entity it's following is centered
	camera.update(dt);
	input.world_pos = input.screen_pos + camera.offset;

	for (auto entity : active_level->entities) {
		update_task(entity, dt);
	}
	
	if (in_dialogue) {
	} else {
		// Deal with the player
		move_entity(g_hero, input.is_down[GLFW_KEY_W], input.is_down[GLFW_KEY_S], input.is_down[GLFW_KEY_A], input.is_down[GLFW_KEY_D]);
		draw_entity(g_hero, Render_Flags::None);
		
		// Handle collisions
		for (auto& entity : active_level->entities) {
			if (entity->get_component<Collision_Component>()) {
				Collision_Element new_collision_element;
				new_collision_element.me = g_hero;
				new_collision_element.other = entity;
				physics_system.collisions.push_back(new_collision_element);
			}
		}
		
		physics_system.process(1.f / 60.f);
	}
	
	frame++;
}
void Game::render() {
	active_level->draw();
	renderer.render_for_frame();
	text_box.render();
}
void Game::exec_console_cmd(char* command_line) {
	char* command = strtok(command_line, " ");
	if (!command) return;
	
	
	if (console.Stricmp(command, "level") == 0) {
		swap_level(this, strtok(NULL, " "));
	}

}
