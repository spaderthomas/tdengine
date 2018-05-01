struct {
	vector<Entity_Visible*> entities;

	Entity_Visible* get_entity(string id) {
		fox_for(ient, entities.size()) {
			Entity_Visible* cur_ent = entities[ient];
			if (cur_ent->id == id) {
				return cur_ent;
			}
		}

		Entity_Visible* new_ent = new Entity_Visible;
		new_ent->id = id;
		entities.push_back(new_ent);
		return new_ent;
	}
} entity_table;