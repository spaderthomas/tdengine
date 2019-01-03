pool_handle<Entity> Level::get_tile(int x, int y) {
	Chunk& chunk = chunks[Chunk_Index(chunk_of(x), chunk_of(y))];
	return chunk.tiles[index_into_chunk(x)][index_into_chunk(y)];
}
void Level::set_tile(pool_handle<Entity> tile, int x, int y) {
	Chunk& chunk = chunks[Chunk_Index(chunk_of(x), chunk_of(y))];
	chunk.tiles[index_into_chunk(x)][index_into_chunk(y)] = tile;
}
EntityHandle Level::get_first_matching_entity(string lua_id) {
	for (auto& entity : entities) {
		if (entity->lua_id == lua_id) return entity;
	}

	return { -1, nullptr };
}
EntityHandle Level::erase_first_matching_entity(string lua_id) {
	for (auto it = entities.begin(); it != entities.end(); it++) {
		auto entity = *it;
		if (entity->lua_id == lua_id) {
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

void add_level(string name) {
	Level* level = new Level;
	level->name = name;
	level->load();
	levels[name] = level;

	// Add the level to the database if it does not already exist 
	string sql_query = "";
	ostringstream query_stream;
	query_stream << "insert into levels(name) ";
	query_stream << "select '" << name << "' ";
	query_stream << "where not exists";
	query_stream << "(select name from levels ";
	query_stream << "where name = '" << name << "')";
	string query = query_stream.str();	
	sqlite3_stmt* statement;
	state_system.sql_wrapper(query.c_str(), &statement, true);
}

void init_levels() {
	const char* sql_query = "select * from levels";
	sqlite3_stmt* statement;
	state_system.sql_wrapper(sql_query, &statement, false);

	do {
		string name = (const char*)sqlite3_column_text(statement, db_schema["levels"]["name"]);
		add_level(name);
	} while (sqlite3_step(statement) != SQLITE_DONE);
}