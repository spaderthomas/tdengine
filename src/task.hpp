struct Action {
	bool is_blocking = false;
	virtual bool update(float dt) = 0;
	virtual void init() {};
};

struct Movement_Action : Action {
	glm::vec2 dest;
	EntityHandle actor;

	bool update(float dt) override;
};

struct Dialogue_Tree;
struct Dialogue_Node;
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
};
