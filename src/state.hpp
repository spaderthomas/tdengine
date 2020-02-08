// Holds the mapping from (state name) -> (entities that care about that state)
std::map<std::string, std::vector<std::string>> state_map;

// Holds the mapping from (state name) -> (state value)
// e.g. the "character_state" table
std::map<std::string, bool> game_state;

void init_state();
void update_state(std::string state_name, bool value);
