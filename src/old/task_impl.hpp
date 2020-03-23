// This is just a factory function that reads table definitions of Actions and sets them up
Action* action_from_table(TableNode* table, EntityHandle actor) {
	Action* action;
	
	std::string kind = tds_string2(table, "kind"); 
	if (kind == "Wait_For_Interaction_Action") {
		action = new Wait_For_Interaction_Action;
	}
	else if (kind == "Dialogue_Action") {
		action = new Dialogue_Action;
	}
	else if (kind == "Movement_Action") {
		action = new Movement_Action;
	}
	else if (kind == "And_Action") {
		action = new And_Action;
   	}
	else if (kind == "Set_State_Action") {
		action = new Set_State_Action;
	}
	else if (kind == "Teleport_Action") {
		action = new Teleport_Action;
	}
	else if (kind == "Camera_Pan_Action") {
		action = new Camera_Pan_Action;
	}
	else if (kind == "Camera_Follow_Action") {
		action = new Camera_Follow_Action;
	}
	else if (kind == "Cutscene_Action") {
		action = new Cutscene_Action;
	}
	else if (kind == "Spin_Action") {
		action = new Spin_Action;
	}
	// @NEXT@ [this one is for the code generator]
	else {
		tdns_log.write("Tried to create an action with an invalid kind: " + kind);
		return nullptr;
	}

	// @hack @spader 9/26/2019: 
	action->actor = actor;
	action->is_blocking = tds_bool2(table, "is_blocking");
	action->init(table);
	return action;
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
	if (!action_queue.size()) return true;
	
	return false;
}

void Task::add_action(Action* action) {
	action_queue.push(action);
}

void Task::init_from_table(TableNode* table, EntityHandle actor) {
	this->actor = actor;
	fox_for(action_idx, table->assignments.size()) {
		TableNode* action_table = tds_table2(table, std::to_string(action_idx));
		Action* action = action_from_table(action_table, actor);
		this->add_action(action);
	}
}
void Task::imgui_visualizer() {
	for (auto action : action_queue.actions) {
		action->imgui_visualizer();
	}
};

// Pushes out a list of connected nodes that ImGui uses to render tasks
std::vector<TaskEditorNode*> make_task_graph(Task* task, ImVec2 base) {
	static int id = 0;
	std::vector<TaskEditorNode*> graph;
	for (auto& action : task->action_queue.actions) {
		TaskEditorNode* node = new TaskEditorNode;
		node->action = action;
		node->pos = base;
		node->id = id++;
		graph.push_back(node);
		
		base = base + ImVec2(100, 0);
	}
	
	return graph;
}
std::vector<TaskEditorNode*> make_task_graph(std::string entity, std::string scene, ImVec2 base) {
	TableNode* task_table = tds_table2(ScriptManager.global_scope, "entity", entity, "scripts", scene);
	EntityHandle dummy = { -1, nullptr };
	Task* task = new Task;
	task->init_from_table(task_table, dummy);
	return make_task_graph(task, base);
}

void TaskEditor::show() {
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
	
	ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;
	
	draw_list->ChannelsSplit(2);
	for (auto* node : task_graph) {
		ImGui::PushID(node->id);
		ImVec2 node_rect_min = offset + node->pos;
		
		const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);
		
		
		// Set the screen position to the bottom of the node
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		
		// Display the node's contents in the foreground channel
		draw_list->ChannelsSetCurrent(1);
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::BeginGroup(); // Lock horizontal position
		std::string kind = node->action->kind();
		ImGui::Text(kind.c_str());
		
		// Switch based on what kind of action we're dealing with -- each has custom GUI
		if (kind == "Movement_Action") {
			Movement_Action* move_action = dynamic_cast<Movement_Action*>(node->action);
			ImGui::SliderFloat2("Destination", glm::value_ptr(move_action->dest), 0, 1);
		}
		ImGui::EndGroup();
		bool node_widgets_active = !old_any_active && ImGui::IsAnyItemActive();
		
		// Set the size of the node to the size of the group containing its contents
		node->size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->size;
		
		// Set the screen position to the bottom left of the node w/o padding
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->size);
		
		bool is_node_being_dragged = ImGui::IsItemActive();
		if (node_widgets_active || is_node_being_dragged)
			node_selected = node;
		if (is_node_being_dragged && ImGui::IsMouseDragging(0))
			node->pos = node->pos + ImGui::GetIO().MouseDelta;
		
		ImU32 node_bg_color = ImGui::IsItemHovered() || node_selected == node
			? IM_COL32(75, 75, 75, 255)
			: IM_COL32(60, 60, 60, 255);
		draw_list->ChannelsSetCurrent(0);
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		
		ImGui::PopID();
	}
	
	draw_list->ChannelsSetCurrent(0);
	fox_for(node_idx, task_graph.size() - 1) {
		TaskEditorNode* node = task_graph[node_idx];
		TaskEditorNode* child = task_graph[node_idx + 1];
		
		ImVec2 p1 = offset + node->pos + ImVec2(node->size.x, node->size.y / 2);
		ImVec2 p2 = offset + child->pos + ImVec2(0, child->size.y / 2);
		draw_list->AddCircleFilled(p1, NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		draw_list->AddCircleFilled(p2, NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		draw_list->AddBezierCurve(p1, p1 + ImVec2(+100, 0), p2 + ImVec2(-100, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f, 1);
	}
	draw_list->ChannelsMerge();
	
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = scrolling + ImGui::GetIO().MouseDelta;
	
	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::End();	
}
