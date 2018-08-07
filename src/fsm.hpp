struct {
	unordered_map<string, vector<State_Component*>> watch_table;

	void update_variable(string variable_name, bool value) {
		Lua.state["state"][variable_name] = value;
		vector<State_Component*>& watchers = watch_table[variable_name];
		for (auto& watcher : watchers) {
			watcher->update(watcher, variable_name, value);
		}
	}

	void register_watcher(string watched_variable, State_Component* state) {
		watch_table[watched_variable].push_back(state);
	}

	bool operator [] (string variable_name) {
		return Lua.state["state"][variable_name];
	}
} knowledge_base;