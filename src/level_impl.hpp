pool_handle<Entity> Level::get_tile(int x, int y) {
	Chunk& chunk = chunks[Chunk_Index(chunk_of(x), chunk_of(y))];
	return chunk.tiles[index_into_chunk(x)][index_into_chunk(y)];
}
void Level::set_tile(pool_handle<Entity> tile, int x, int y) {
	Chunk& chunk = chunks[Chunk_Index(chunk_of(x), chunk_of(y))];
	chunk.tiles[index_into_chunk(x)][index_into_chunk(y)] = tile;
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
	for (auto it : chunks) {
		Chunk& chunk = it.second;
		fox_for(itilex, CHUNK_SIZE) {
			fox_for(itiley, CHUNK_SIZE) {
				EntityHandle entity = chunk.tiles[itilex][itiley];
				draw_entity(entity, Render_Flags::None);
			}
		}
	}

	for (auto entity : entities) {
		draw_entity(entity, Render_Flags::None);
	}
}

// The chunk index is used as an index into the JSON save object
// @leak: We make a table to save the level but never free any of it
void Level::save() {
	TableNode* table = new TableNode;
	tds_set2(table, new TableNode, "entities");
	TableNode* saved_entities = tds_table2(table, "entities");\
	
	for (auto& entity : entities) {
		TableNode* saved_entity = entity->save();
		saved_entities->push_back(saved_entity);
	}

	// @hack: Would be better to just give the writer the name of the table.
	TableWriter writer;
	writer.write_line("levels." + name + " = "); 
	writer.same_line();
	table->dump(writer);

	string out_path = absolute_path(path_join({"src", "scripts", "levels", name + ".tds"}));
	writer.dump(out_path);
#if 0
	json j;
	for (auto it : chunks) {
		Chunk_Index index = it.first;
		string index_key = to_string(index.x) + "," + to_string(index.y);
		Chunk& chunk = it.second;
		fox_for(itilex, CHUNK_SIZE) {
			fox_for(itiley, CHUNK_SIZE) {	
				Entity* tile = chunk.tiles[itilex][itiley]();
				if (tile != nullptr) { tile->save(j["chunks"][index_key][itilex][itiley]); }
				else { j["chunks"][index_key][itilex][itiley] = "NULL"; }
			}
		}
	}

	int indx = 0;
	for (auto handle : entities) {
		handle->save(j["entities"][indx++]);
	}
	
	string path = absolute_path("save\\" + name + ".json");
	ofstream save_file(path);
	save_file << std::setw(4) << j << std::endl;
#endif
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
	#if 0
	json j;
	ifstream level_file(absolute_path("save\\" + name + ".json"));
	if (!level_file.good()) return;
	level_file >> j;

	// Load tile chunks
	for (json::iterator it = j["chunks"].begin(); it != j["chunks"].end(); it++) {
		// Parse the chunk indices out of the string (which is of the form "x,y"
		stringstream index_stream = stringstream(it.key());
		string index;
		getline(index_stream, index, ',');
		int chunk_x = stoi(index);
		getline(index_stream, index, ',');
		int chunk_y = stoi(index);

		// Fill in the tiles in the chunk
		Chunk& chunk = chunks[Chunk_Index(chunk_x, chunk_y)];
		json chunk_as_json = it.value();
		fox_for(itilex, CHUNK_SIZE) {
			fox_for(itiley, CHUNK_SIZE) {
				auto tile_json = chunk_as_json[itilex][itiley];
				if (tile_json != "NULL") {
					string name = tile_json["lua_id"];
					pool_handle<Entity> new_ent = Entity::create(name);
					new_ent->load(tile_json);
					chunk.tiles[itilex][itiley] = new_ent;
				}
				else {
					chunk.tiles[itilex][itiley] = { -1, nullptr };
				}
			}
		}
	}

	// Load entities
	entities.clear();
	for (unsigned int i = 0; i < j["entities"].size(); i++) {
		json entity_json = j["entities"][i];
		string name = entity_json["lua_id"];

		pool_handle<Entity> handle = Entity::create(name);
		handle->load(entity_json);
		entities.push_back(handle);
	}
	#endif
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
