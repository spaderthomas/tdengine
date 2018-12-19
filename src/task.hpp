struct Action {
	EntityHandle actor;
	bool is_blocking = false;
	virtual bool update(float dt) = 0;
	virtual void init() {};
	virtual string kind() = 0;
};
Action* action_from_table(sol::table table, EntityHandle actor);
void init_is_blocking(Action* action, sol::table& table);

// Conjunction of actions which blocks until all are complete
struct And_Action : Action {
	vector<Action*> actions;
	bool update(float dt) override;
	string kind() override { return "And_Action"; }
};

struct Movement_Action : Action {
	glm::vec2 dest;

	bool update(float dt) override;
	string kind() override { return "Movement_Action"; }
};

struct Wait_For_Interaction_Action : Action {
	bool update(float dt) override;
	string kind() override { return "Wait_For_Interaction_Action"; }
};

struct Dialogue_Action : Action {
	Dialogue_Tree* tree;
	bool update(float dt) override;
	void init() override;
	string kind() override { return "Dialogue_Action"; }
};

struct Set_State_Action : Action {
	string var;
	bool value;

	bool update(float dt) override;
	string kind() override { return "Set_State_Action"; }
};

struct Teleport_Action : Action {
	float x;
	float y;

	bool update(float dt) override;
	string kind() override { return "Teleport_Action"; }
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
	EntityHandle actor;

	bool update(float dt);
	void add_action(Action* a);
	void init_from_table(sol::table table, EntityHandle actor);
};

struct TaskEditorNode {
	TaskEditorNode() { action = nullptr; sibling = nullptr; }
	Action* action;
	vector<TaskEditorNode*> children;
	TaskEditorNode* sibling;
	ImVec2 pos;
	ImVec2 size;
	int id;
}; 

TaskEditorNode* make_task_graph(Task* task, ImVec2 base) {
	static int id = 0;
	TaskEditorNode* node = nullptr;
	TaskEditorNode* prev = nullptr;
	TaskEditorNode* root = nullptr;
	for (auto& action : task->action_queue.actions) {
		node = new TaskEditorNode;
		if (!root) root = node;
		node->action = action;
		node->pos = base;
		node->id = id++;

		// Construct the vertical list of inner actions if it's a conjunction
		if (action->kind() == "And_Action") {
			ImVec2 inner_base = base;
			And_Action* and_action = dynamic_cast<And_Action*>(action);
			TaskEditorNode* inner_node = nullptr;
			TaskEditorNode* inner_prev = nullptr;
			for (auto& inner_action : and_action->actions) {
				inner_node = new TaskEditorNode;
				inner_node->action = inner_action;
				inner_node->pos = inner_base;
				inner_node->id = id++;

				node->children.push_back(inner_node);
				if (inner_prev) 
					inner_prev->sibling = inner_node;
				inner_prev = inner_node;
				inner_base = inner_base + ImVec2(0, 20);
			}
		}

		if (prev)
			prev->children.push_back(node);
		prev = node;
		base = base + ImVec2(20, 0);
	}

	return root;
}

struct TaskEditor {
	const float NODE_SLOT_RADIUS = 4.0f;
	const ImVec2 NODE_WINDOW_PADDING = ImVec2(8.0f, 8.0f);
	ImVec2 scrolling = ImVec2(0.f, 0.f);
	int selected = -1;
	TaskEditorNode* task_graph;

	void draw_graph(TaskEditorNode* node, ImDrawList* draw_list) {
		if (!node) return;

		ImGui::PushID(node->id);
		ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;
		ImVec2 node_rect_min = offset + node->pos;

		const float NODE_SLOT_RADIUS = 4.0f;
		const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

		draw_list->ChannelsSetCurrent(1); // Foreground
		bool old_any_active = ImGui::IsAnyItemActive();

		// Set the screen position to the bottom of the node
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);

		// Display the node's contents
		ImGui::BeginGroup(); // Lock horizontal position
		ImGui::Text(node->action->kind().c_str());
		ImGui::EndGroup();
		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());

		// Set the size of the node to the size of the group containing its contents
		node->size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->size;

		draw_list->ChannelsSetCurrent(0);
		// Set the screen position to the bottom left of the node w/o padding
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->size);

		int node_hovered_in_scene = -1;
		if (ImGui::IsItemHovered())
			node_hovered_in_scene = node->id;

		bool is_node_being_dragged = ImGui::IsItemActive();
		if (node_widgets_active || is_node_being_dragged)
			selected = node->id;
		if (is_node_being_dragged && ImGui::IsMouseDragging(0))
			node->pos = node->pos + ImGui::GetIO().MouseDelta;

		ImU32 node_bg_color = node_hovered_in_scene == node->id
			? IM_COL32(75, 75, 75, 255)
			: IM_COL32(60, 60, 60, 255);
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		
		ImGui::PopID();

		// Draw next nodes
		draw_graph(node->sibling, draw_list);
		for (auto child : node->children) draw_graph(child, draw_list);
	}
	void draw_links(TaskEditorNode* node, ImDrawList* draw_list) {
		if (!node) return;

		ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;
		draw_list->ChannelsSetCurrent(0);
		
		for (auto& child : node->children) {
			ImVec2 p1 = offset + node->pos + ImVec2(node->size.x, node->size.y / 2);
			ImVec2 p2 = offset + child->pos + ImVec2(0, child->size.y / 2);
			draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
		}
		TaskEditorNode* sibling = node->sibling;
		if (sibling) {
			ImVec2 p1 = offset + node->pos + ImVec2(node->size.x / 2, node->size.y);
			ImVec2 p2 = offset + sibling->pos + ImVec2(sibling->size.x / 2, 0);
			draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
		}

		draw_links(node->sibling, draw_list);
		for (auto child : node->children) draw_links(child, draw_list);
	}
	void show() {
		ImGui::Begin("Task Editor", 0, ImGuiWindowFlags_AlwaysAutoResize);

		// Create child canvas
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
		ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
		ImGui::PushItemWidth(120.0f);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// Display grid
		ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
		float GRID_SZ = 64.0f;
		ImVec2 win_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetWindowSize();
		for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
			draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
		for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
			draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);

		draw_list->ChannelsSplit(2);
		draw_graph(task_graph, draw_list);
		draw_links(task_graph, draw_list);
		draw_list->ChannelsMerge();

		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
			scrolling = scrolling + ImGui::GetIO().MouseDelta;

		ImGui::PopItemWidth();
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
		ImGui::End();


	}

};
