pool_handle<Entity> Level::get_tile(int x, int y) {
	Chunk& chunk = chunks[Chunk_Index(chunk_of(x), chunk_of(y))];
	return chunk.tiles[index_into_chunk(x)][index_into_chunk(y)];
}
void Level::set_tile(pool_handle<Entity> tile, int x, int y) {
	Chunk& chunk = chunks[Chunk_Index(chunk_of(x), chunk_of(y))];
	chunk.tiles[index_into_chunk(x)][index_into_chunk(y)] = tile;
}

void Level::draw() {
	for (auto it : chunks) {
		Chunk& chunk = it.second;
		fox_for(itilex, CHUNK_SIZE) {
			fox_for(itiley, CHUNK_SIZE) {
				pool_handle<Entity> handle = chunk.tiles[itilex][itiley];
				if (handle) handle->draw(Render_Flags::None);
			}
		}
	}

	for (auto handle : entities) {
		if (handle) handle->draw(Render_Flags::None);
	}
}

// The chunk index is used as an index into the JSON save object
void Level::save() {
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
}

//@leak We never free up any tiles that were previously allocated.
void Level::load() {
	json j;
	ifstream level_file(absolute_path("save\\" + name + ".json"));
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
					string lua_id = tile_json["lua_id"];
					pool_handle<Entity> new_ent = Entity::create(lua_id);
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
		string lua_id = entity_json["lua_id"];

		pool_handle<Entity> handle = Entity::create(lua_id);
		handle->load(entity_json);
		entities.push_back(handle);
	}
}


void init_levels() {
	const char* sql_query = "select * from levels";
	sqlite3_stmt* statement;
	state_system.sql_wrapper(sql_query, &statement, false);

	do {
		string name = (const char*)sqlite3_column_text(statement, db_schema["levels"]["name"]);

		Level* level = new Level;
		level->name = name;
		level->load();
		levels[name] = level;

	} while (sqlite3_step(statement) != SQLITE_DONE);
}