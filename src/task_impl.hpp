Action* action_from_table(sol::table table, EntityHandle actor) {
	string kind = table["kind"];
	if (kind == "Wait_For_Interaction_Action") {
		Wait_For_Interaction_Action* action = new Wait_For_Interaction_Action;
		action->is_blocking = table["is_blocking"];
		action->actor = actor;
		return action;
	}
	else if (kind == "Dialogue_Action") {
		Dialogue_Tree* tree = new Dialogue_Tree;
		sol::table dialogue = table["dialogue"];
		tree->init_from_table(dialogue);

		Dialogue_Action* action = new Dialogue_Action;
		action->init();
		action->is_blocking = true;
		action->tree = tree;
		action->actor = actor;
		return action;
	}
	else if (kind == "Movement_Action") {
		Movement_Action* action = new Movement_Action;
		action->dest.x = table["dest"]["x"];
		action->dest.y = table["dest"]["y"];
		action->actor = actor;
		return action;
	}
	else if (kind == "And_Action") {
		And_Action* action = new And_Action;
		action->is_blocking = true;
		sol::table actions = table["actions"];
		for (auto& kvp : actions) {
			sol::table action_definition = kvp.second;
			action->actions.push_back(action_from_table(action_definition, actor));
		}

		return action;
	}

	tdns_log.write("Tried to create an action with an invalid kind: " + kind);
	return nullptr;
}

bool And_Action::update(float dt) {
	bool done = true;
	for (auto it = actions.begin(); it != actions.end();) {
		Action* action = *it;
		if (action->update(dt)) {
			it = actions.erase(it);
		}
		else {
			done = false;
			it++;
		}
	}

	return done;
}

bool Movement_Action::update(float dt) {
	def_get_cmp(mc, actor.deref(), Movement_Component);
	def_get_cmp(pc, actor.deref(), Position_Component);

	if (vec_almost_equals(pc->world_pos, dest)) {
		return true;
	}

	bool up = pc->world_pos.y < dest.y;
	bool down = pc->world_pos.y > dest.y;
	bool right = pc->world_pos.x < dest.x;
	bool left = pc->world_pos.x > dest.x;

	move_entity(actor, up, down, left, right);
	return false;
}	

bool Wait_For_Interaction_Action::update(float dt) {
	def_get_cmp(ic, actor.deref(), Interaction_Component);
	if (ic->was_interacted_with) {
		ic->on_interact(actor, ic->other);

		// Clear the IC so we don't call this again next frame.
		ic->was_interacted_with = false;
		ic->other = { 0, nullptr };

		return true;
	}

	return false;
}

void Dialogue_Action::init() {
	is_blocking = true;
}
bool Dialogue_Action::update(float dt) {
	Dialogue_Node* node = tree->traverse();
	if (game.input.was_pressed(GLFW_KEY_1)) {
		node->set_response(0);
	}
	else if (game.input.was_pressed(GLFW_KEY_2)) {
		node->set_response(1);
	}
	else if (game.input.was_pressed(GLFW_KEY_3)) {
		node->set_response(2);
	}
	else if (game.input.was_pressed(GLFW_KEY_4)) {
		node->set_response(3);
	}

	node->show_line();

	// Probably keep this here. 
	if (game.input.was_pressed(GLFW_KEY_SPACE)) {
		// If the dialogue has fully shown
		if (text_box.is_all_text_displayed()) {
			// Break if node is terminal
			if (node->terminal) {
				text_box.reset_and_hide();
				return true;
			}
			// Show responses if it is not
			else {
				string all_response_text;
				for (auto& response : node->responses) {
					all_response_text += response + "\r";
				}
				text_box.begin(all_response_text);
			}
		}
		// If the set has shown fully (but not ALL dialogue), go to the next set
		else if (text_box.is_current_set_displayed()) {
			text_box.resume();
		}
		// If the dialogue has partially shown, skip to the end of the line set
		else {
			Line_Set& set = text_box.current_set();
			set.point = set.max_point;
		}
	}

	return false;
}

bool Task::update(float dt) {
	// Run through actions until we reach one which blocks or the queue is empty
	Action* next_action;
	while (true) {
		next_action = action_queue.next();
		if (!next_action) break;

		if (next_action->update(dt)) {
			action_queue.remove(next_action);
		}

		if (next_action->is_blocking) break;
	}

	action_queue.reset_top(); // Reset the index to the top of the queue

	// If there are no more actions, this task is done
	if (!action_queue.actions.size()) return true;

	return false;
}

void Task::add_action(Action* a) {
	action_queue.push(a);
}

void Task::init_from_table(sol::table table, EntityHandle actor) {
	this->actor = actor;

	for (auto it : table) {
		sol::table action_table = it.second.as<sol::table>();
		Action* action = action_from_table(action_table, actor);
		this->add_action(action);
	}
}