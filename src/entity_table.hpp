struct {
	vector<Entity*> entities;

	Entity* get_entity(string id) {
		fox_for(ient, entities.size()) {
			Entity* cur_ent = entities[ient];
			if (cur_ent->id == id) {
				return cur_ent;
			}
		}

		Entity* new_ent = new Entity;
		new_ent->id = id;
		entities.push_back(new_ent);
		return new_ent;
	}
} entity_table;