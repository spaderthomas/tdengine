struct Action {
	EntityHandle actor;
	bool is_blocking = false;
	virtual bool update(float dt) = 0;
	virtual void init(TableNode* table) {};
	virtual string kind() = 0;
	virtual void imgui_visualizer() = 0;
	virtual void set_actor(EntityHandle actor) {};
};
Action* action_from_table(TableNode* table, EntityHandle actor);
void init_is_blocking_tds(Action* action, TableNode* table);

struct Action_Queue {
	vector<Action*> actions;
	int index = 0;

	Action* operator[](int i) {
		return actions[i];
	}

	void push(Action* action) {
		actions.push_back(action);
	}

	void push_front(Action* action) {
		actions.insert(actions.begin(), action);
	}

	Action* next() {
		if (index < (int)actions.size()) {
			Action* ret = actions[index];
			index++;
			return ret;
		}
		return nullptr;
	}

	void remove(Action* action) {
		for (auto it = actions.begin(); it != actions.end();) {
			if (*it == action) {
				it = actions.erase(it);
				if (it - actions.begin() <= index)  
					index = fox_max(index - 1, 0);
			} else {
				it++;
			}
		}
	}

	int size() {
		return actions.size();
	}

	void reset_top() {
		index = 0;
	}
};

struct Task {
	Action_Queue action_queue;
	EntityHandle actor;

	bool update(float dt);
	void add_action(Action* a);
	void init_from_table(TableNode* table, EntityHandle actor);
	void imgui_visualizer();
};

struct TaskEditorNode {
	TaskEditorNode() { action = nullptr; pos = ImVec2(0, 0); size = ImVec2(0, 0); }
	Action* action;
	ImVec2 pos;
	ImVec2 size;
	int id;
}; 

vector<TaskEditorNode*> make_task_graph(Task* task, ImVec2 base);
vector<TaskEditorNode*> make_task_graph(string entity, string scene, ImVec2 base);
struct TaskEditor {
	const float NODE_SLOT_RADIUS = 4.0f;
	const ImVec2 NODE_WINDOW_PADDING = ImVec2(8.0f, 8.0f);
	ImVec2 scrolling = ImVec2(0.f, 0.f);
	vector<TaskEditorNode*> task_graph;
	TaskEditorNode* node_selected;
	
	void show();
};
