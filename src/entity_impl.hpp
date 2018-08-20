template<typename Component_Type>
component_handle Entity::add_component() {
	component_handle handle = component_pool.next_available();
	components[&typeid(Component_Type)] = handle;
	return handle;
}

template <typename Component_Type>
bool Entity::remove_component() {
	component_handle handle = components[&typeid(Component_Type)];
	component_pool.mark_available(handle);
}

template <typename Component_Type>
Component_Type* Entity::get_component() {
	component_handle handle = components[&typeid(Component_Type)];
	return (Component_Type*)component_pool.get(handle);
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
		if (it.second.get_type() == sol::type::table) {
			sol::table table = it.second.as<sol::table>();
			if (component_type == "Graphic_Component") {
				component_handle handle = entity->add_component<Graphic_Component>();
				Graphic_Component* component = component_pool.get<Graphic_Component>(handle);
				new(component) Graphic_Component;
				component->init_from_table(table);
			}
			else if (component_type == "Bounding_Box") {
				component_handle handle = entity->add_component<Bounding_Box>();
				Bounding_Box* component = component_pool.get<Bounding_Box>(handle);
				new(component) Bounding_Box;
				component->init_from_table(table);
			}
			else if (component_type == "Position_Component") {
				component_handle handle = entity->add_component<Position_Component>();
				Position_Component* component = component_pool.get<Position_Component>(handle);
				new(component) Position_Component;
				component->init_from_table(table);
			}
			else if (component_type == "Movement_Component") {
				component_handle handle = entity->add_component<Movement_Component>();
				Movement_Component* component = component_pool.get<Movement_Component>(handle);
				component->wish = glm::vec2(0.f);
			}
			else if (component_type == "Vision") {
				component_handle handle = entity->add_component<Vision>();
				Vision* component = component_pool.get<Vision>(handle);
				new(component) Vision;
				component->init_from_table(table);
			}
			else if (component_type == "Interaction_Component") {
				component_handle handle = entity->add_component<Interaction_Component>();
				Interaction_Component* component = component_pool.get<Interaction_Component>(handle);
				new(component) Interaction_Component;
				component->init_from_table(table);
			}
			else if (component_type == "State_Component") {
				component_handle handle = entity->add_component<State_Component>();
				State_Component* component = component_pool.get<State_Component>(handle);
				new(component) State_Component;
				component->init_from_table(table);
			}
		}
	}

	return entity;
}

void Entity::draw(Render_Flags flags) {
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
	for (auto& kvp : components) {
		//component->save(j["Components"][icomponent++]);
	}
}

void Entity::load(json& entity) {
	auto components_json = entity["Components"];
	for (auto component_json : components_json) {
		if (component_json["kind"] == "Position_Component") {
			component_handle handle = this->add_component<Position_Component>();
			Position_Component* component = component_pool.get<Position_Component>(handle);
			component->load(component_json);
		}
	}
}

