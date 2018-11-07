template<typename Component_Type>
pool_handle<any_component> Entity::add_component() {
	// Grab a handle to a chunk of memory in the pool, initialize it with inplace new, and then mark it in the mapping for this entity
	pool_handle<any_component> handle = component_pool.next_available();
	any_component* any = handle();
	new(any) Component_Type;
	components[&typeid(Component_Type)] = handle;
	return handle;
}

template <typename Component_Type>
bool Entity::remove_component() {
	pool_handle handle = components[&typeid(Component_Type)];
	component_pool.mark_available(handle);
}

// Use this in C++ where templated functions are available
template <typename Component_Type>
Component_Type* Entity::get_component() {
	pool_handle<any_component> handle = components[&typeid(Component_Type)];

	// If it returns a zeroed out entry, then the component doesn't exist
	if (handle.pool == nullptr) {
		return nullptr;
	}

	// Otherwise, try to grab the component associated with the handle (performing all the safety checks of the handle)
	// We can cast because it's just a union.
	any_component* any = handle();
	return (Component_Type*)any;
}

// Use this in Lua where they are not
any_component* Entity::get_component(string kind) {
	const type_info* info = component_map[kind];
	pool_handle<any_component> handle = components[info];
	any_component* thing = handle();
	return thing;
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

pool_handle<Entity> Entity::create(string lua_id) {
	// Creates a template entity from the given Lua ID
	pool_handle<Entity> entity_handle = entity_pool.next_available();
	Entity* entity = entity_handle();
	entity->id = next_id++;
	entity->lua_id = lua_id;

	sol::table lua_components = Entity::get_definition(lua_id);
	for (auto it : lua_components) {
		string component_type = it.first.as<string>();
		if (it.second.get_type() == sol::type::table) {
			sol::table table = it.second.as<sol::table>();
			if (component_type == "Graphic_Component") {
				pool_handle<any_component> handle = entity->add_component<Graphic_Component>();
				Graphic_Component* component = &handle()->graphic_component;
				component->init_from_table(table);
			}
			else if (component_type == "Position_Component") {
				pool_handle<any_component> handle = entity->add_component<Position_Component>();
				Position_Component* component = &handle()->position_component;
				component->init_from_table(table);
			}
			else if (component_type == "Movement_Component") {
				pool_handle<any_component> handle = entity->add_component<Movement_Component>();
				Movement_Component* component = &handle()->movement_component;
				component->wish = glm::vec2(0.f);
			}
			else if (component_type == "Vision") {
				pool_handle<any_component> handle = entity->add_component<Vision>();
				Vision* component = &handle()->vision;
				component->init_from_table(table);
			}
			else if (component_type == "Interaction_Component") {
				pool_handle<any_component> handle = entity->add_component<Interaction_Component>();
				Interaction_Component* component = &handle()->interaction_component;
				component->init_from_table(table);
			}
			else if (component_type == "State_Component") {
				pool_handle<any_component> handle = entity->add_component<State_Component>();
				State_Component* component = &handle()->state_component;
				component->init_from_table(table);

				sol::table watched_variables = table["watched_variables"];
				for (auto& watched : watched_variables) {
					string variable = watched.second.as<string>();
					knowledge_base.register_watcher(watched.second.as<string>(), handle);
				}
			}
			else if (component_type == "Door_Component") {
				pool_handle<any_component> handle = entity->add_component<Door_Component>();
				Door_Component* component = &handle()->door_component;
				component->init_from_table(table);
			}
			else if (component_type == "Collision_Component") {
				pool_handle<any_component> handle = entity->add_component<Collision_Component>();
				Collision_Component* component = &handle()->collision_component;
				component->init_from_table(table);
			}
			else if (component_type == "Action_Component") {
				pool_handle<any_component> handle = entity->add_component<Action_Component>();
				Action_Component* component = &handle()->action_component;
				component->init_from_table(table);
			}
		}
	}

	return entity_handle;
}

void Entity::draw(Render_Flags flags) {
	auto graphic_component = get_component<Graphic_Component>();
	auto position_component = get_component<Position_Component>();
	if (graphic_component && position_component) {
		renderer.draw(graphic_component, position_component, flags);
	}
};
void Entity::save(json& j) {
	j["lua_id"] = lua_id;
	int icomponent = 0;
	for (auto& kvp : components) {
		auto handle = kvp.second;
		Component* component = (Component*)handle();

		if (component) {
			component->save(j["Components"][icomponent++]);
		}
	}
}
void Entity::load(json& entity) {
	auto components_json = entity["Components"];
	for (auto& component_json : components_json) {
		string kind = component_json["kind"];
		if (kind == "NULL") {
			continue; // NULL just means a component which doesn't require loading
		}
		Component* component = (Component*)get_component(kind);
		component->load(component_json);
	}
}
void Entity::destroy(pool_handle<Entity> handle) {
	handle()->clear_components();
	entity_pool.mark_available(handle);
}


// Lua defs
string entity_name(EntityHandle entity) {
	return entity->lua_id;
}
int entity_id(EntityHandle entity) {
	return entity->id;
}
void on_collide(EntityHandle me, EntityHandle other) {
	def_get_cmp(cc, me, Collision_Component);
	cc->on_collide(me, other);
}
void update_animation(EntityHandle me, float dt) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	auto anim = gc->active_animation;
	anim->time_to_next_frame -= dt;
	if (anim->time_to_next_frame <= 0.f) {
		anim->next_frame();
	}
}
int collider_kind(EntityHandle me) {
	def_get_cmp(cc, me.deref(), Collision_Component);
	return cc ?
		cc->kind :
		Collider_Kind::NO_COLLIDER;
}