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
