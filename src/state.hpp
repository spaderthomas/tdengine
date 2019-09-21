// Holds the mapping from (state name) -> (entities that care about that state)
map<string, vector<string>> state_map;

// Holds the mapping from (state name) -> (state value)
// e.g. the "character_state" table
map<string, bool> game_state;

void init_state();
void update_state(string state_name, bool value);
