unordered_map<const char*, unordered_map<const char*, int>> db_schema = {
	{
		"levels", {{"name", 0}}
	},
	{
		"entities", {{"lua_id", 0}, {"game_id", 1}, {"level", 2}}
	},
	{
		"position_components", {{"x", 0}, {"y", 1}, {"entity", 2}}
	}
}