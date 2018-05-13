struct {
	vector<Entity_Visible*> entities;
	vector<Entity_Visible*> background_tiles;

	Entity_Visible* get_entity(string id) {
		fox_for(ient, entities.size()) {
			Entity_Visible* cur_ent = entities[ient];
			if (cur_ent->id == id) {
				return cur_ent;
			}
		}

		Entity_Visible* new_ent = new Entity_Visible;
		new_ent->id = id;
		new_ent->layer = FOREGROUND;
		entities.push_back(new_ent);
		return new_ent;
	}

	Entity_Visible* get_background_tile(string id) {
		fox_for(ient, background_tiles.size()) {
			Entity_Visible* cur_tile = background_tiles[ient];
			if (cur_tile->id == id) {
				return cur_tile;
			}
		}

		Entity_Visible* new_tile = new Entity_Visible;
		new_tile->id = id;
		new_tile->layer = BACKGROUND;
		background_tiles.push_back(new_tile);
		return new_tile;
	}

} entity_table;