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
void Entity::clear_components() {
	for (auto kvp : components) {
		pool_handle handle = kvp.second;
		component_pool.mark_available(handle);
	}
}

template <typename Component_Type>
Component_Type* Entity::get_component() {
	pool_handle<any_component> handle = components[&typeid(Component_Type)];

	// If it returns a zeroed out entry, then the component doesn't exist
	if (!handle.pool) {
		return nullptr;
	}

	// Otherwise, try to grab the component associated with the handle (performing all the safety checks of the handle)
	any_component* any = handle();
	return (Component_Type*)any;
}
any_component* Entity::get_component(string kind) {
	const type_info* info = component_map[kind];
	pool_handle<any_component> handle = components[info];
	any_component* thing = handle();
	return thing;
}

pool_handle<Entity> Entity::create(string entity_name) {
	// Creates a template entity from the given Lua ID
	pool_handle<Entity> entity_handle = entity_pool.next_available();
	Entity* entity = entity_handle();
	entity->id = next_id++;
	entity->lua_id = entity_name;

	TableNode* components = tds_table(ScriptManager.global_scope, "entity", entity_name, "components");
	for (auto& node : components->assignments) {
		KVPNode* kvp = (KVPNode*)node;
		string& type = kvp->key;
		TableNode* component_table = (TableNode*)(kvp->value);

		if (type == "Graphic_Component") {
			pool_handle<any_component> handle = entity->add_component<Graphic_Component>();
			Graphic_Component* component = &handle()->graphic_component;
			component->init_from_table(component_table);

			//@hack: I would feel better initializing this in the table?
			//       But since I want all component modifying stuff to take in an entity, I can't 
			//       directly modify the GC from within itself without duplicating the code for setting animation
			//       or doing this. Not sure?
			set_animation(entity_handle, tds_string(component_table, "default_animation"));
			// Set the scaling of this based on the first sprite we see. Right now, no objects resize (i.e. all sprites it could use are the same dimensions)
			// Also, use 640x360 because the raw dimensions are based on this
			Sprite* default_sprite = component->get_current_frame();
			component->scale = glm::vec2((float)default_sprite->width / (float)640,
				(float)default_sprite->height / (float)360);

		}
		else if (type == "Position_Component") {
			pool_handle<any_component> handle = entity->add_component<Position_Component>();
			Position_Component* component = &handle()->position_component;
			component->init_from_table(component_table);
		}
		else if (type == "Movement_Component") {
			pool_handle<any_component> handle = entity->add_component<Movement_Component>();
			Movement_Component* component = &handle()->movement_component;
			component->init_from_table(component_table);
			component->wish = glm::vec2(0.f);
		}
		else if (type == "Vision") {
			pool_handle<any_component> handle = entity->add_component<Vision_Component>();
			Vision_Component* component = &handle()->vision;
			component->init_from_table(component_table);
		}
		else if (type == "Interaction_Component") {
			pool_handle<any_component> handle = entity->add_component<Interaction_Component>();
			Interaction_Component* component = &handle()->interaction_component;
			component->init_from_table(component_table);
		}
		else if (type == "Door_Component") {
			pool_handle<any_component> handle = entity->add_component<Door_Component>();
			Door_Component* component = &handle()->door_component;
			component->init_from_table(component_table);
		}
		else if (type == "Collision_Component") {
			pool_handle<any_component> handle = entity->add_component<Collision_Component>();
			Collision_Component* component = &handle()->collision_component;
			component->init_from_table(component_table);
		}
		else if (type == "Task_Component") {
			pool_handle<any_component> handle = entity->add_component<Task_Component>();
			Task_Component* component = &handle()->task_component;
			component->init_from_table(component_table);

			// Get the entity's state from the database and initialize the task it says
			string entity_state = state_system.entity_state(entity_name);
			Task* task = new Task;
			task->init_from_table(tds_table(ScriptManager.global_scope, "entity", entity_name, "scripts", entity_state), entity_handle);
			component->task = task;
		}
	}

	return entity_handle;
}

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

