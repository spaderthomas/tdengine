/*
First, something happens in the game which triggers an event. An event is just a variable that's in the 
knowledge base, plus a new value for it to take on. You trigger an event by calling

knowledge_base.update_variable(variable_name, value)

The knowledge engine then updates the actual mappings which hold all the information for the game. This
is just a table defined in state.lua. Then, it goes into the watch table. The watch table is how a variable
registers itself as needing to update when a variable changes. The watch table is just a mapping from strings
(i.e. variables) to vectors of state components (i.e. all the things that need to be updated when that 
variable changes). 

For each watching component, it just calls an update function which is defined in Lua. 
*/

struct {
	unordered_map<string, vector< pool_handle<any_component>> > watch_table;

	void update_variable(string variable_name, bool value) {
		// Update the store of facts in Lua
		set_variable(variable_name, value);

		// Run all the callbacks of things registered to watch this variable
		vector<pool_handle<any_component>>& watchers = watch_table[variable_name];
		for (auto& watcher : watchers) {
			State_Component* state = &watcher->state_component;
			state->update(state, variable_name, value);
		}
	}

	void register_watcher(string watched_variable, pool_handle<any_component> state) {
		watch_table[watched_variable].push_back(state);
	}

	bool operator [] (string variable_name) {
		return Lua.state[variable_name];
	}

	void set_variable(string variable_name, bool value) {
		Lua.state[variable_name.c_str()] = value;
	}
} knowledge_base;