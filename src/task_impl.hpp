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
