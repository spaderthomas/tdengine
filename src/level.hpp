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
	size_t operator()(const Chunk_Index& chunk) const {
		return (chunk.x + chunk.y) * (chunk.x + chunk.y + 1) / 2 + chunk.y;
	}
};

#define CHUNK_SIZE 8
#define chunk_of(x) ((x) / CHUNK_SIZE)
#define index_into_chunk(x) ((x) % CHUNK_SIZE)
struct Chunk {
	pool_handle<Entity> tiles[CHUNK_SIZE][CHUNK_SIZE];
};

struct Level {
	string name;
	unordered_map<Chunk_Index, Chunk> chunks;
	vector<pool_handle<Entity>> entity_handles;
	int count_entities();

	pool_handle<Entity> get_tile(int x, int y);
	void set_tile(pool_handle<Entity> handle, int x, int y);
	void draw();
	void save();
	void load();
};
unordered_map<string, Level*> levels;

Level* get_level(string name);