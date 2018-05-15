struct {
	vector<Entity*> entities;

	Entity* get_entity(string id) {
		fox_for(ient, entities.size()) {
			Entity* cur_ent = entities[ient];
			if (cur_ent->id == id) {
				return cur_ent;
			}
		}
		return 0;
	}

	void add_entity(Entity* ent) {
		entities.push_back(ent);
	}

} entity_table;
