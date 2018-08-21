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
				Entity* ent = entity_pool.get(chunk.tiles[itilex][itiley]);
				if (ent) {
					ent->draw(Render_Flags::None);
				}
			}
		}
	}

	for (auto handle : entity_handles) {
		entity_pool.get(handle)->draw(Render_Flags::None);
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
				Entity* tile = entity_pool.get(chunk.tiles[itilex][itiley]);
				if (tile != nullptr) { tile->save(j["chunks"][index_key][itilex][itiley]); }
				else { j["chunks"][index_key][itilex][itiley] = "NULL"; }
			}
		}
	}

	int indx = 0;
	for (auto handle : entity_handles) {
		entity_pool.get(handle)->save(j["entities"][indx++]);
	}

	string path = string("../../save/") + name + string(".json");
	ofstream save_file(path);
	save_file << std::setw(4) << j << std::endl;
}

//@leak We never free up any tiles that were previously allocated.
void Level::load() {
	json j;
	string path = "../../save/" + name + ".json";
	ifstream load_file(path);
	load_file >> j;

	// Load tile chunks
	for (json::iterator it = j["chunks"].begin(); it != j["chunks"].end(); it++) {
		// Parse the chunk indices out of the string (which is of the form "x,y"
		stringstream index_stream = stringstream(it.key());
		string index;
		getline(index_stream, index, ',');
		int chunk_x = stoi(index);
		getline(index_stream, index, ',');
		int chunk_y = stoi(index);

		Chunk& chunk = chunks[Chunk_Index(chunk_x, chunk_y)];
		json chunk_as_json = it.value();
		fox_for(itilex, CHUNK_SIZE) {
			fox_for(itiley, CHUNK_SIZE) {
				auto tile_json = chunk_as_json[itilex][itiley];
				if (tile_json != "NULL") {
					pool_handle<Entity> new_ent = Entity::create(tile_json);
					chunk.tiles[itilex][itiley] = new_ent;
				}
				else {
					chunk.tiles[itilex][itiley] = { -1, nullptr };
				}
			}
		}
	}

	// Load entities
	entity_handles.clear();
	for (unsigned int i = 0; i < j["entities"].size(); i++) {
		json entity_as_json = j["entities"][i];
		pool_handle<Entity> handle = Entity::create(entity_as_json);
		entity_handles.push_back(handle);
	}
}

map<string, Level*> levels;
void init_levels() {
	string dir = "..\\..\\save";
	for (directory_iterator iter(dir); iter != directory_iterator(); ++iter) {
		if (is_regular_file(iter->status())) {
			string path = iter->path().string();
			Level* level = new Level;
			level->name = strip_extension(name_from_full_path(path));
			levels[level->name] = level;
		}
	}
}