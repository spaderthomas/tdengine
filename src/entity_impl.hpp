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
void Entity::remove_component() {
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

EntityHandle Entity::create(string entity_name) {
	// Creates a template entity from the given Lua ID
	EntityHandle entity = entity_pool.next_available();
	entity->id = next_id++;
	entity->name = entity_name;
	
	TableNode* components = tds_table2(ScriptManager.global_scope, "entity", entity_name, "components");
	for (auto& node : components->assignments) {
		KVPNode* kvp = (KVPNode*)node;
		auto type = kvp->key;
		auto table = (TableNode*)(kvp->value);

		auto component = Component::create(type, table);

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
	for (auto& kvp : components) {
		pool_handle<any_component> handle = kvp.second;
		Component* component = (Component*)handle();
		if (component) component->imgui_visualizer();
	}
}

void init_hero() {
	g_hero = Entity::create("boon");
}
