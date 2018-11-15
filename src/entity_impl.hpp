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

				//@hack: I would feel better initializing this in the table?
				//       But since I want all component modifying stuff to take in an entity, I can't 
				//       directly modify the GC from within itself without duplicating the code for setting animation
				//       or doing this. Not sure?
				set_animation(entity_handle, table["default_animation"]);
				// Set the scaling of this based on the first sprite we see. Right now, no objects resize (i.e. all sprites it could use are the same dimensions)
				// Also, use 640x360 because the raw dimensions are based on this
				Sprite* default_sprite = component->get_current_frame();
				component->scale = glm::vec2((float)default_sprite->width / (float)640, 
											 (float)default_sprite->height / (float)360);

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
				pool_handle<any_component> handle = entity->add_component<Vision_Component>();
				Vision_Component* component = &handle()->vision;
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
			else if (component_type == "Update_Component") {
				pool_handle<any_component> handle = entity->add_component<Update_Component>();
				Update_Component* component = &handle.deref()->update_component;
				component->init_from_table(table);
			}
		}
	}

	return entity_handle;
}

// @c-style
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

// Internal API 
Points_Box get_vision_box(EntityHandle me) {
	def_get_cmp(pc, me.deref(), Position_Component);
	def_get_cmp(vision, me.deref(), Vision_Component);

	Points_Box vision_box;
	vision_box.top = pc->world_pos.y + vision->depth;
	vision_box.bottom = pc->world_pos.y;
	vision_box.left = pc->world_pos.x - vision->width / 2.f;
	vision_box.right = pc->world_pos.x + vision->width / 2.f;

	return vision_box;
}
Points_Box get_bounding_box_world(EntityHandle me) {
	def_get_cmp(pc, me.deref(), Position_Component);
	def_get_cmp(cc, me.deref(), Collision_Component);
	fox_assert(cc);
	fox_assert(pc);

	Points_Box bounding_box;
	bounding_box.top = pc->world_pos.y + cc->bounding_box.screen_center.y + cc->bounding_box.screen_extents.y / 2;
	bounding_box.bottom = pc->world_pos.y + cc->bounding_box.screen_center.y - cc->bounding_box.screen_extents.y / 2;
	bounding_box.right = pc->world_pos.x + cc->bounding_box.screen_center.x + cc->bounding_box.screen_extents.x / 2;
	bounding_box.left = pc->world_pos.x + cc->bounding_box.screen_center.x - cc->bounding_box.screen_extents.x / 2;

	return bounding_box;
}