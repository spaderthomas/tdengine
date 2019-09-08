// Holds the mapping from (state name) -> (entities that care about that state)
map<string, vector<string>> state_map;

// Holds the mapping from (state name) -> (state value)
// e.g. the "character_state" table
map<string, bool> game_state;

void init_state() {
	// Make sure to do this so that we can re-call this function to reset stuff
	state_map.clear();
	game_state.clear();
	
	// Load the game state table from TDS and put it into a C++ map
	TableNode* game_state_table = tds_table(GAME_STATE_KEY);
	for (KVPNode* kvp : game_state_table->assignments) {
		string key = kvp->key;
		bool value = tds_bool2(game_state_table, key);
		game_state[key] = value;
	}
	
	// Setup the mapping from pieces of state to entities that must be notified when that state changes
	TableNode* stateful_entities = tds_table(CH_STATE_KEY);

	for (auto& kvp : stateful_entities->assignments) {
		// Find the entity's current state
		string entity_name = kvp->key;
		string entity_state = tds_string(CH_STATE_KEY, entity_name);

		// Grab the variables it cares about to transition to a new state from its current one
		TableNode* transitions = tds_table(ENTITY_KEY, entity_name, STATE_KEY, entity_state, TRANSITIONS_KEY);

		// Mark this entity down in the map entry for each of those variables
		fox_for(idx, transitions->assignments.size()) {
			TableNode* vars = tds_table2(transitions, to_string(idx), VARS_KEY);

			for (auto& kvp : vars->assignments) {
				vector<string>& map_for_this_var = state_map[kvp->key];
				map_for_this_var.push_back(entity_name);
			}
		}
	}
}

void update_state(string state_name, bool value) {
	// Set it directly in the script tree so if we save it later, it's up to date
	tds_set2(ScriptManager.global_scope, value, "game_state", state_name);

	// Set it in our in-memory CPP friendly structure
	game_state[state_name] = value;
	
	// Get the list of entities that have registered themselves to this state variable
	auto& entities_to_check = state_map[state_name];
	for (auto& entity_name : entities_to_check) {
		// Grab the entity's current state
		const string& current_state = tds_string2(ScriptManager.global_scope, CH_STATE_KEY, entity_name);
		TableNode* transitions = tds_table(ENTITY_KEY, entity_name, STATE_KEY, current_state, TRANSITIONS_KEY);

		fox_for(idx, transitions->assignments.size()) {
			TableNode* vars = tds_table2(transitions, to_string(idx), VARS_KEY);

			bool should_transition = true;
			for (KVPNode* kvp : vars->assignments) {
				const string& state_name = kvp->key;
				bool state_val_needed = tds_bool2(vars, state_name);
				bool actual_state_val = tds_bool(GAME_STATE_KEY, state_name);

				if (state_val_needed != actual_state_val) {
					should_transition = false;
					break;
				}
			}

			if (should_transition) {
				// Update the character state table
				string next_state = tds_string2(transitions, to_string(idx), NEXT_STATE_KEY);
				tds_set(next_state, CH_STATE_KEY, entity_name);

				// Update entities that've been instantiated
				// @note: This fails if the level has no entities. This is fine, just add a check here in case
				// we're in the editor or something and we haven't loaded the game yet.
				//
				// Also, we should probably do this for each layer
				auto& entities = game.active_level->entities;
				for (auto entity : entities) {
					if (entity->name == entity_name) {
						TableNode* task_table = tds_table(ENTITY_KEY, entity_name, SCRIPTS_KEY, next_state);
						auto task = entity->get_component<Task_Component>()->task;
						task->init_from_table(task_table, entity);
					}
				}
			}
			
		}
	}
}
