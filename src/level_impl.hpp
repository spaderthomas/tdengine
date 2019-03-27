EntityHandle Level::get_tile(int x, int y) {
	for (auto& tile : tiles) {
		auto tc = tile->get_component<TileComponent>();
		if (tc->x == x && tc->y == y) {
			return tile;
		}
	}

	return NULL_ENTITY;
}
void Level::set_tile(EntityHandle tile, int x, int y) {
	// If there was another tile in this spot, erase it
	for (auto it = tiles.begin(); it != tiles.end(); ++it) {
		auto tc = (*it)->get_component<TileComponent>();
		if (tc->x == x && tc->y == y) {
			tiles.erase(it);
			break;
		}
	}

	if (tile) {
		auto tc = tile->get_component<TileComponent>();
		tc->x = x;
		tc->y = y;
		tiles.push_back(tile);
	}
}

EntityHandle Level::get_first_matching_entity(string name) {
	for (auto& entity : entities) {
		if (entity->name == name) return entity;
	}

	return { -1, nullptr };
}
EntityHandle Level::erase_first_matching_entity(string name) {
	for (auto it = entities.begin(); it != entities.end(); it++) {
		auto entity = *it;
		if (entity->name == name) {
			entities.erase(it);
			return entity;
		}
	}

	return { -1, nullptr };
}

void Level::draw() {
	vector<EntityHandle>* containers[2] = {&tiles, &entities};
	for (auto container : containers) {
		for (auto entity : *container) {
			draw_entity(entity, Render_Flags::None);
		}
	}
	
}

// @leak: We make a table to save the level but never free any of it
void Level::save() {
	// Make a new table to hold the saved data
	TableNode* table = new TableNode;

	// Save the entities to this table
	tds_set2(table, new TableNode, "entities");
	TableNode* saved_entities = tds_table2(table, "entities");\
	
	for (auto& entity : entities) {
		TableNode* saved_entity = entity->save();
		saved_entities->push_back(saved_entity);
	}

	
	// Save the tiles to this table
	tds_set2(table, new TableNode, "tiles");
	TableNode* saved_tiles = tds_table2(table, "tiles");
	for (auto tile : tiles) {
		TableNode* saved_tile = tile->save();
		saved_tiles->push_back(saved_tile);
	}

	TableWriter writer;
	writer.table = table;
	writer.table_name = "levels." + name + " = ";
	writer.dump(absolute_path(path_join({"src", "scripts", "levels", name + ".tds"})));
}

//@leak We never free up any tiles that were previously allocated.
void Level::load() {
	entities.clear();
	TableNode* entities_table = tds_table(LEVELS_KEY, name, ENTITIES_KEY);

	fox_for(idx, entities_table->assignments.size()) {
		TableNode* entity_table = tds_table2(entities_table, to_string(idx));
		string name = tds_string2(entity_table, NAME_KEY);
		EntityHandle handle = Entity::create(name);
		handle->load(entity_table);
		entities.push_back(handle);
	}
}

// Just a little wrapper that checks the table that has the names of all the levels
// and loads them up. Simple, dumb, but works good. 
void init_levels() {
	TableNode* level_names = tds_table(LEVELS_KEY, NAMES_KEY);
	fox_for(idx, level_names->assignments.size()) {
		string level_name = tds_string2(level_names, to_string(idx));

		// Load the level into memory
		Level* level = new Level;
		level->name = level_name;
		level->load();
		levels[level_name] = level;
	}
}
