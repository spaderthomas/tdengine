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

pool_handle<Entity> Entity::create(string entity_name) {
	// Creates a template entity from the given Lua ID
	EntityHandle entity = entity_pool.next_available();
	entity->id = next_id++;
	entity->name = entity_name;
	
	TableNode* components = tds_table2(ScriptManager.global_scope, "entity", entity_name, "components");
	for (auto& node : components->assignments) {
		KVPNode* kvp = (KVPNode*)node;
		string& type = kvp->key;
		TableNode* component_table = (TableNode*)(kvp->value);

		if (type == "Graphic_Component") {
			auto handle = entity->add_component<Graphic_Component>();
			Graphic_Component* component = &handle()->graphic_component;
			component->init_from_table(component_table);
			
			//@hack: I would feel better initializing this in the table?
			//       But since I want all component modifying stuff to take in an entity, I can't 
			//       directly modify the GC from within itself without duplicating the code for setting animation
			//       or doing this. Not sure?
			set_animation(entity, tds_string2(component_table, "default_animation"));
			// Set the scaling of this based on the first sprite we see. Right now, no objects resize (i.e. all sprites it could use are the same dimensions)
			// Also, use 640x360 because the raw dimensions are based on this
			Sprite* default_sprite = component->get_current_frame();
			component->scale = glm::vec2((float)default_sprite->width / (float)640,
										 (float)default_sprite->height / (float)360);
			
		}
		else if (type == "Position_Component") {
			entity->add_component<Position_Component>();
		}
		else if (type == "Movement_Component") {
			auto handle = entity->add_component<Movement_Component>();
			Movement_Component* component = &handle()->movement_component;
			component->init_from_table(component_table);
			component->wish = glm::vec2(0.f);
		}
		else if (type == "Vision") {
			auto handle = entity->add_component<Vision_Component>();
			Vision_Component* component = &handle()->vision;
			component->init_from_table(component_table);
		}
		else if (type == "Interaction_Component") {
			entity->add_component<Interaction_Component>();
		}
		else if (type == "Door_Component") {
			auto handle = entity->add_component<Door_Component>();
			Door_Component* component = &handle()->door_component;
			component->init_from_table(component_table);
		}
		else if (type == "Collision_Component") {
			auto handle = entity->add_component<Collision_Component>();
			Collision_Component* component = &handle()->collision_component;
			component->init_from_table(component_table);
		}
		else if (type == "Task_Component") {
			auto handle = entity->add_component<Task_Component>();
			Task_Component* component = &handle()->task_component;
			component->init_from_table(component_table);
			
			// Get the entity's current state and initialize the corresponding task
			string entity_state = tds_string(CH_STATE_KEY, entity_name);
			TableNode* task_table = tds_table(ENTITY_KEY, entity_name, SCRIPTS_KEY, entity_state);
			
			Task* task = new Task;
			task->init_from_table(task_table, entity);
			component->task = task;
		}
		else if (type == "Battle_Component") {
			auto handle = entity->add_component<Battle_Component>();
			Battle_Component* component = &handle()->battle_component;
			component->init_from_table(component_table);
		}
		else if (type == "Tile_Component") {
			entity->add_component<Tile_Component>();
		}
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
