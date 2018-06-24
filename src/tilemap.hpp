struct Chunk_Index {
	int x;
	int y;

	Chunk_Index(int x, int y) {
		this->x = x;
		this->y = y;
	}

	bool operator==(const Chunk_Index &other) const {
		return (x == other.x && y == other.y);
	}
};

// https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
// Implementation of a bijective mapping from NxN to N
template <>
struct hash<Chunk_Index> {
	std::size_t operator()(const Chunk_Index& chunk) const {
		return (chunk.x + chunk.y) * (chunk.x + chunk.y + 1) / 2 + chunk.y;
	}
};

#define CHUNK_SIZE 8
#define chunk_of(x) ((x) / CHUNK_SIZE)
#define index_into_chunk(x) ((x) % CHUNK_SIZE)
struct Chunk {
	Entity* tiles[CHUNK_SIZE][CHUNK_SIZE];
};

struct Level {
	string name;
	unordered_map<Chunk_Index, Chunk> chunks;
	vector<Entity*> entities;

	Entity* get_tile(int x, int y) {
		Chunk& chunk = chunks[Chunk_Index(chunk_of(x), chunk_of(y))];
		return chunk.tiles[index_into_chunk(x)][index_into_chunk(y)];
	}

	void set_tile(Entity* tile, int x, int y) {
		Chunk& chunk = chunks[Chunk_Index(chunk_of(x), chunk_of(y))];
		chunk.tiles[index_into_chunk(x)][index_into_chunk(y)] = tile;
	}

	void draw() {
		for (auto it : chunks) {
			Chunk& chunk = it.second;
			fox_for(itilex, CHUNK_SIZE) {
				fox_for(itiley, CHUNK_SIZE) {
					Entity* ent = chunk.tiles[itilex][itiley];
					if (ent) {
						ent->draw();
					}
				}
			}
		}
	}

	// The chunk index is used as an index into the JSON save object
	void save() {
		json j;
		for (auto it : chunks) {
			Chunk_Index index = it.first;
			string index_key = to_string(index.x) + "," + to_string(index.y);
			Chunk& chunk = it.second;
			fox_for(itilex, CHUNK_SIZE) {
				fox_for(itiley, CHUNK_SIZE) {
					Entity* tile = chunk.tiles[itilex][itiley];
					if (tile != nullptr) { tile->save(j[index_key][itilex][itiley]); }
					else { j[index_key][itilex][itiley] = "NULL"; }
				}
			}
		}

		string path = string("../../save/") + name + string(".json");
		ofstream save_file(path);
		save_file << std::setw(4) << j << std::endl;
	}

	//@leak We never free up any tiles that were previously allocated.
	void load() {
		string path = "../../save/" + name + ".json";
		ifstream load_file(path);
		json j;

		load_file >> j;
		for (json::iterator it = j.begin(); it != j.end(); it++) {
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
					// Grab the entry and check if it's the special key we use for nonexistent entry
					auto tile_json = chunk_as_json[itilex][itiley];
					if (tile_json != "NULL") {
						// Get the tile type, use it to create a new template entity
						string type_name = tile_json["lua_id"];
						Entity* new_ent = Basic_Tile::create(type_name);
						// Load in the specific instance's information from JSON
						new_ent->load(tile_json);
						chunk.tiles[itilex][itiley] = new_ent;
					}
					else {
						chunk.tiles[itilex][itiley] = nullptr;
					}
				}
			}
		}
	}
};