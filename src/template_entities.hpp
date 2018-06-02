// We use this to store a basic entity of each kind loaded from Lua, and the create function needed to copy it.
// We need the basic entity so we know what to render to ImGui buttons
vector<Entity*> template_tiles;
vector<string> tile_lua_ids = {
	"tree",
	"grass",
	"grass_flower1",
	"grass_flower2",
	"grass_flower3",
	"fence",
	"sand",
	"sand_cracked"
};

void init_template_entities() {
	for (auto it = tile_lua_ids.begin(); it != tile_lua_ids.end(); ++it) {
		template_tiles.push_back(Basic_Tile::create(*it));
	}
}

Basic_Tile* get_template_tile(string lua_id) {
	for (auto it = template_tiles.begin(); it != template_tiles.end(); ++it) {
		if ((*it)->lua_id == lua_id) {
			return (Basic_Tile*)(*it);
		}
	}
}
