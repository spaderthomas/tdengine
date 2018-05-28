vector<Entity*> template_entities;
void init_template_entities() {
	for (auto it = create_methods.begin(); it != create_methods.end(); ++it) {
		Entity* (*create_func)() = (Entity* (*)())it->second;
		template_entities.push_back(create_func());
	}
}

template <typename Entity_Type>
Entity_Type* get_template_entity() {
	for (auto it = template_entities.begin(); it != template_entities.end(); ++it) {
		Entity_Type* cast_entity = dynamic_cast<Entity_Type*>(*it);
		if (cast_entity) {
			return cast_entity;
		}
	}
}
