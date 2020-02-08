template <typename Component_Type>
void Entity::remove_component() {
	auto handle = components[&typeid(Component_Type)];
	component_pool.mark_available(handle);
	components.erase(&typeid(Component_Type));
}
void Entity::clear_components() {
	for (auto kvp : components) {
		pool_handle handle = kvp.second;
		component_pool.mark_available(handle);
	}

	this->components.clear();
}

template <typename Component_Type>
Component_Type* Entity::get_component() {
	auto handle = components[&typeid(Component_Type)];
	
	// If it returns a zeroed out entry, then the component doesn't exist
	if (!handle.pool) {
		return nullptr;
	}
	
	// Otherwise, try to grab the component associated with the handle (performing all the safety checks of the handle)
	return (Component_Type*)handle();
}

Component* Entity::get_component(std::string kind) {
	const type_info* info = component_map[kind];
	auto handle = components[info];
	
	// @spader 10/7/2019 Unsafe, probably, I just want a pointer to the union. Since every class in the union derives from
	// Component, it's all good. 
	return (Component*)handle(); 
}

EntityHandle Entity::create(std::string entity_name) {
	// Creates a template entity from the given Lua ID
	EntityHandle entity = entity_pool.next_available();
	entity->id = next_id++;
	entity->name = entity_name;
	
	TableNode* components = tds_table2(ScriptManager.global_scope, "entity", entity_name, "components");
	for (auto& node : components->assignments) {
		KVPNode* kvp = (KVPNode*)node;
		auto type = kvp->key;
		auto table = (TableNode*)(kvp->value);

		auto component = create_component(type, table);
		entity->components[component_map[type]] = component;
	}

	// @spader 10/7/2019: Would be nice if this were "pure" or generic, but I'm not sure where else to do component setup
	// that requires an entity to be around. It's for the component, not really the entity, so I don't want it in Component::create.
	// But I don't like it here because it feels hacky. 
	if_component(task_component, entity, Task_Component) {
		// Get the entity's current state and initialize the corresponding task
		std::string entity_state = tds_string(CH_STATE_KEY, entity_name);
		TableNode* task_table = tds_table(ENTITY_KEY, entity_name, SCRIPTS_KEY, entity_state);
			
		Task* task = new Task;
		task->init_from_table(task_table, entity);
		task_component->task = task;
	}
	
	return entity;
}

TableNode* Entity::save() {
	TableNode* self = new TableNode;
	tds_set2(self, name, "name");
	
	TableNode* components_table = new TableNode;
	tds_set2(self, components_table, "components");
	for (auto& kvp : components) {
		auto& handle = kvp.second;
		Component* component = (Component*)handle();
		// 9/8/2019 @hack: If you check for existence, you push a null component back to the vector. Which would make
		// this null when you try to save it, because it doesn't really exist. Feels hacky. 
		if (!component) continue;
		TableNode* saved = component->save();
		// Components that have no state to save just return nullptr
		// So if we get that, just ignore saving this component.
		if (saved) tds_set2(components_table, saved, component->name());
	}
	
	return self;
}
void Entity::load(TableNode* table) {
	TableNode* components_table = tds_table2(table, COMPONENTS_KEY);
	for (KVPNode* kvp : components_table->assignments) {
		Component* component = (Component*)get_component(kvp->key);
		TableNode* component_table = tds_table2(components_table, kvp->key);
		component->load(component_table);
	}
}
void Entity::destroy(pool_handle<Entity> handle) {
	handle()->clear_components();
	entity_pool.mark_available(handle);
}

void Entity::imgui_visualizer() {
	// Iterate through components, displaying whatever you need
	for (auto& [type, handle] : components) {
		Component* component = (Component*)handle();
		if (component) component->imgui_visualizer();
	}
}

void init_hero() {
	g_hero = Entity::create("boon");
}
