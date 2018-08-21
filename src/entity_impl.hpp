template<typename Component_Type>
pool_handle<any_component> Entity::add_component() {
	pool_handle handle = component_pool.next_available();
	components[&typeid(Component_Type)] = handle;
	return handle;
}

template <typename Component_Type>
bool Entity::remove_component() {
	pool_handle handle = components[&typeid(Component_Type)];
	component_pool.mark_available(handle);
}

template <typename Component_Type>
Component_Type* Entity::get_component() {
	pool_handle<any_component> handle = components[&typeid(Component_Type)];
	return (Component_Type*)component_pool.get(handle);
}

void Entity::clear_components() {
	for (auto kvp : components) {
		pool_handle handle = kvp.second;
		component_pool.mark_available(handle);
	}
}


sol::table Entity::get_definition(string lua_id) {
	string script = Lua.definitions_to_script[lua_id];
	return Lua.state[script][lua_id];
}

// Pulls defaults from Lua, then fills in whatever was saved
pool_handle<Entity> Entity::create(json& entity_json) {
	//@note 
	// if you don't explicitly make this a string, it recursively calls
	// this create (since the result is a json object), then promptly crashes
	string lua_id = entity_json["lua_id"];
	pool_handle<Entity> handle = Entity::create(lua_id);
	Entity* entity = entity_pool.get(handle);
	entity->load(entity_json);
	return handle;
}
pool_handle<Entity> Entity::create(string lua_id) {
	pool_handle<Entity> entity_handle = entity_pool.next_available();
	Entity* entity = entity_pool.get(entity_handle);
	entity->id = next_id++;
	entity->lua_id = lua_id;

	sol::table lua_components = Entity::get_definition(lua_id);
	for (auto it : lua_components) {
		string component_type = it.first.as<string>();
		if (it.second.get_type() == sol::type::table) {
			sol::table table = it.second.as<sol::table>();
			if (component_type == "Graphic_Component") {
				pool_handle<any_component> handle = entity->add_component<Graphic_Component>();
				Graphic_Component* component = &component_pool.get(handle)->graphic_component;
				new(component) Graphic_Component;
				component->init_from_table(table);
				if (entity->lua_id == "boon") {
					int a = 1;
				}

			}
			else if (component_type == "Bounding_Box") {
				pool_handle<any_component> handle = entity->add_component<Bounding_Box>();
				Bounding_Box* component = &component_pool.get(handle)->bounding_box;
				new(component) Bounding_Box;
				component->init_from_table(table);
			}
			else if (component_type == "Position_Component") {
				pool_handle<any_component> handle = entity->add_component<Position_Component>();
				Position_Component* component = &component_pool.get(handle)->position_component;
				new(component) Position_Component;
				component->init_from_table(table);
			}
			else if (component_type == "Movement_Component") {
				pool_handle<any_component> handle = entity->add_component<Movement_Component>();
				Movement_Component* component = &component_pool.get(handle)->movement_component;
				component->wish = glm::vec2(0.f);
			}
			else if (component_type == "Vision") {
				pool_handle<any_component> handle = entity->add_component<Vision>();
				Vision* component = &component_pool.get(handle)->vision;
				new(component) Vision;
				component->init_from_table(table);
			}
			else if (component_type == "Interaction_Component") {
				pool_handle<any_component> handle = entity->add_component<Interaction_Component>();
				Interaction_Component* component = &component_pool.get(handle)->interaction_component;
				new(component) Interaction_Component;
				component->init_from_table(table);
			}
			else if (component_type == "State_Component") {
				pool_handle<any_component> handle = entity->add_component<State_Component>();
				State_Component* component = &component_pool.get(handle)->state_component;
				new(component) State_Component;
				component->init_from_table(table);
			}
		}
	}

	if (entity->lua_id == "boon") {
		auto gc = entity->get_component<Graphic_Component>();
		int a = 1;
	}
	return entity_handle;
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
			pool_handle handle = this->add_component<Position_Component>();
			Position_Component* component = &component_pool.get(handle)->position_component;
			component->load(component_json);
		}
	}
}
void Entity::destroy(pool_handle<Entity> handle) {
	Entity* entity = entity_pool.get(handle);
	entity->clear_components();
	entity_pool.mark_available(handle);
}
