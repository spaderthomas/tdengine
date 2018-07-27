template <typename Component_Type>
void Entity::add_component(Component_Type* c) {
	this->remove_component<Component_Type>();
	components.push_back(c);
}

template <typename Component_Type>
bool Entity::remove_component() {
	for (auto it = components.begin(); it != components.end(); it++) {
		if (dynamic_cast<Component_Type*>(*it)) {
			components.erase(it);
			return true;
		}
	}

	return false;
}

template <typename Component_Type>
Component_Type* Entity::get_component() const {
	for (auto it = components.begin(); it != components.end(); ++it) {
		Component_Type* cast_component = dynamic_cast<Component_Type*>(*it);
		if (cast_component) { return cast_component; }
	}

	return nullptr;
}

sol::table Entity::get_definition(string lua_id) {
	string script = Lua.definitions_to_script[lua_id];
	return Lua.state[script][lua_id];
}

// Pulls defaults from Lua, then fills in whatever was saved
Entity* Entity::create(json& entity_json) {
	//@note 
	// if you don't explicitly make this a string, it recursively calls
	// this create (since the result is a json object), then promptly crashes
	string lua_id = entity_json["lua_id"];
	Entity* new_entity = Entity::create(lua_id);
	new_entity->load(entity_json);
	return new_entity;
}

Entity* Entity::create(string lua_id) {
	Entity* entity = new Entity;
	entity->id = next_id++;
	entity->lua_id = lua_id;

	sol::table lua_components = Entity::get_definition(lua_id);
	for (auto it : lua_components) {
		string component_type = it.first.as<string>();
		sol::table table = it.second.as<sol::table>();
		if (component_type == "Graphic_Component") {
			Graphic_Component* gc = new Graphic_Component;
			gc->init_from_table(table);
			entity->add_component(gc);
		}
		else if (component_type == "Bounding_Box") {
			Bounding_Box* box = new Bounding_Box;
			box->init_from_table(table);
			entity->add_component(box);
		}
		else if (component_type == "Position_Component") {
			Position_Component* pc = new Position_Component;
			pc->init_from_table(table);
			entity->add_component(pc);
		}
		else if (component_type == "Movement_Component") {
			Movement_Component* mc = new Movement_Component;
			mc->wish = glm::vec2(0.f);
			entity->add_component(mc);
		}
		else if (component_type == "Vision") {
			Vision* vision = new Vision;
			vision->init_from_table(table);
			entity->add_component(vision);
		}
		else if (component_type == "Interaction_Component") {
			Interaction_Component* ic = new Interaction_Component;
			ic->init_from_table(table);
			entity->add_component(ic);
		}
	}

	return entity;
}

void Entity::draw(Render_Flags flags) const {
	auto graphic_component = get_component<Graphic_Component>();
	auto position_component = get_component<Position_Component>();
	if (graphic_component && position_component) {
		renderer.draw(graphic_component, position_component, flags);
	}
	else {
		string msg = "Tried to draw entity, but component was undefined. Entity number: " + to_string(id);
		tdns_log.write(msg);
	}
};

void Entity::save(json& j) {
	j["lua_id"] = lua_id;
	int icomponent = 0;
	for (auto& component : components) {
		component->save(j["Components"][icomponent++]);
	}
}

void Entity::load(json& entity) {
	auto components_json = entity["Components"];
	for (auto component : components_json) {
		if (component["kind"] == "Position_Component") {
			Position_Component* pc = new Position_Component;
			pc->load(component);
			this->add_component(pc);
		}
	}
}

