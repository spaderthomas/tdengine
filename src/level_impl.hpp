EntityHandle Level::get_tile(int x, int y) {
	for (auto& tile : tiles) {
		auto tc = tile->get_component<Tile_Component>();
		if (tc->x == x && tc->y == y) {
			return tile;
		}
	}

	return NULL_ENTITY;
}
void Level::set_tile(EntityHandle tile, int x, int y) {
	// If there was another tile in this spot, erase it
	for (auto it = tiles.begin(); it != tiles.end(); ++it) {
		auto tc = (*it)->get_component<Tile_Component>();
		if (tc->x == x && tc->y == y) {
			tiles.erase(it);
			break;
		}
	}

	if (tile) {
		auto tc = tile->get_component<Tile_Component>();
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
void Level::clear_entities() {
	entities.clear(); // @leak
}
void Level::create_or_add_entities(TableNode* entities_table) {
	fox_for(i, entities_table->assignments.size()) {
		TableNode* entity_table = tds_table2(entities_table, to_string(i));
		string name = tds_string2(entity_table, NAME_KEY);

		// Look for an existing entity of this name
		auto it = std::find_if(entities.begin(), entities.end(), [&name](auto& entity) {
			return entity->name == name;
		});

		// If we find it, just update it with the parameters from the table
		if (it != entities.end()) {
			tdns_log.write("Just loaded an existing " + name + " for a cutscene");
			auto entity = *it;
			entity->load(entity_table);
		}
		// If we don't, make a fresh one and load some data from the table
		else {
			tdns_log.write("Just added a " + name + " for a cutscene");
			auto entity = Entity::create(name);
			entity->load(entity_table);
			entities.push_back(entity);	
		}
	}
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
	
	// Write it out to a file
	TableWriter writer;
	writer.table = table;
	writer.table_name = "levels." + name + " = ";
	auto level_file = absolute_path(path_join({"src", "scripts", "levels", name + ".tds"}));
	writer.dump(level_file);

	// Write the updated level to the in-memory TDS tree
	tds_del(LEVELS_KEY, this->name);
	ScriptManager.parse(level_file);
}

void Level::load_entities(TableNode* entities_table) {
	entities.clear();

	fox_for(idx, entities_table->assignments.size()) {
		TableNode* entity_table = tds_table2(entities_table, to_string(idx));
		string name = tds_string2(entity_table, NAME_KEY);
		EntityHandle handle = Entity::create(name);
		handle->load(entity_table);
		entities.push_back(handle);
	}	
}

//@leak We never free up any tiles that were previously allocated.
void Level::load() {
	load_entities(tds_table(LEVELS_KEY, name, ENTITIES_KEY));

	tiles.clear();
	TableNode* tiles_table = tds_table(LEVELS_KEY, name, TILES_KEY);

	fox_for(idx, tiles_table->assignments.size()) {
		TableNode* tile_table = tds_table2(tiles_table, to_string(idx));
		string name = tds_string2(tile_table, NAME_KEY);
		EntityHandle tile = Entity::create(name);
		tile->load(tile_table);
		tiles.push_back(tile);
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

bool is_valid_level_name(string name) {
	for (auto& kvp : levels) {
		if (kvp.first == name) {
			return true;
		}
	}
	
	return false;
}

void swap_level(Layer* layer, string name) {
	if (!is_valid_level_name(name)) {
		layer->console.AddLog("Invalid level name");
		return;
	}
		
	layer->active_level = levels[name];
}
