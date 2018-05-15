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
};
