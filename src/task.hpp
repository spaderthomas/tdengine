struct Action {
	EntityHandle actor;
	bool is_blocking = false;
	virtual bool update(float dt) = 0;
	virtual void init() {};
};
Action* action_from_table(sol::table table, EntityHandle actor);

// Conjunction of actions which blocks until all are complete
struct And_Action : Action {
	vector<Action*> actions;
	bool update(float dt) override;
};

struct Movement_Action : Action {
	glm::vec2 dest;

	bool update(float dt) override;
};

struct Wait_For_Interaction_Action : Action {
	bool update(float dt) override;
};

struct Dialogue_Action : Action {
	Dialogue_Tree* tree;
	bool update(float dt) override;
	void init() override;
};


struct Action_Queue {
	deque<Action*> actions;
	int index = 0;

	void push(Action* action) {
		actions.push_back(action);
	}

	void push_front(Action* action) {
		actions.push_front(action);
	}

	Action* next() {
		if (index < actions.size()) {
			Action* ret = actions[index];
			index++;
			return ret;
		}
		return nullptr;
	}

	void remove(Action* action) {
		for (auto iter = actions.begin(); iter != actions.end();) {
			if (*iter == action) {
				iter = actions.erase(iter);
			} else {
				iter++;
			}
		}
	}

	void reset_top() {
		index = 0;
	}
};
struct Task {
	Action_Queue action_queue;
	bool update(float dt);
	void add_action(Action* a);
	void init_from_table(sol::table table, EntityHandle actor);
};

