// We need the basic entity so we know what to render to ImGui buttons
// We don't -need- to split these up, but we're doing so now to differentiate for the editor
vector<Entity*> template_entities;
vector<string> entities = {
	"tree",
	"fence_left",
	"fence_middle",
	"fence_right",
	"fence_top",
	"fence_post",
	"cactus",
	"bush",
	"building",
};

//@leak hotloading
void init_template_entities() {
	template_entities.clear();
	for (auto it = entities.begin(); it != entities.end(); ++it) {
		Entity* entity = Entity::create(*it);
		
		template_entities.push_back(entity);
	}
}

Entity* get_template_tile(string lua_id) {
	for (auto it = template_entities.begin(); it != template_entities.end(); ++it) {
		if ((*it)->lua_id == lua_id) {
			return *it;
		}
	}

	return nullptr;
}
