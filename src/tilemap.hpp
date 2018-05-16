struct Tilemap {
	Entity* tiles[128][128];

	void draw() {
		fox_for(irow, 128) {
			fox_for(icol, 128) {
				if (tiles[irow][icol]) {
					tiles[irow][icol]->draw(glm::ivec2(irow, icol));
				}
			}
		}
	}

	void save() {
		json tilemap_as_json;
		fox_for(irow, 128) {
			fox_for(icol, 128) {
				Entity* tile = tiles[irow][icol];
				if (tile) {
					tilemap_as_json[irow][icol] = tile->id;
				}
				else {
					tilemap_as_json[irow][icol] = "";
				}
			}
		}

		ofstream save_file("../../save/save.json");
		save_file << tilemap_as_json;
	}

	void load() {
		ifstream load_file("../../save/save.json");
		json tilemap_as_json;
		load_file >> tilemap_as_json;
		fox_for(irow, 128) {
			fox_for(icol, 128) {
				string id = tilemap_as_json[irow][icol];
				if (id != "") {
					tiles[irow][icol] = entity_table.get_entity(id);
				}
				else {
					tiles[irow][icol] = 0;
				}
			}
		}

		ofstream out_stream("../../save/save.json");
		out_stream << tilemap_as_json;
	}

};
