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


// Lua defs
string entity_name(EntityHandle entity) {
	return entity->lua_id;
}
int entity_id(EntityHandle entity) {
	return entity->id;
}
void on_collide(EntityHandle me, EntityHandle other) {
	def_get_cmp(cc, me, Collision_Component);
	if (cc)
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
void draw_entity(EntityHandle me, Render_Flags flags) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	def_get_cmp(pc, me.deref(), Position_Component);
	if (gc && pc) {
		renderer.draw(gc, pc, flags);
	}
}
void teleport_entity(EntityHandle me, float x, float y) {
	def_get_cmp(pc, me.deref(), Position_Component);
	if (pc)
		pc->world_pos = { x, y };
}
void move_entity(EntityHandle me, float dx, float dy) {
	def_get_cmp(mc, me.deref(), Movement_Component);
	mc->wish += glm::vec2(dx, dy);

	// Tell the Physics system to check + resolve if this movement causes collisions
	physics_system.movers.push_back(me);
}
void set_animation(EntityHandle me, string wish_name) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	for (auto& animation : gc->animations) {
		if (animation->name == wish_name) {
			gc->active_animation = animation;
			gc->active_animation->icur_frame = 0;
			return;
		}
	}

	string msg = "Tried to set active animation to " + wish_name + " but it was not registered in the component!";
	tdns_log.write(msg);
}
void set_animation2(EntityHandle me, string wish_name) {
	def_get_cmp(gc, me.deref(), Graphic_Component);
	if (gc->active_animation) {
		if (gc->active_animation->name == wish_name) {
			return;
		}
	}

	set_animation(me, wish_name);
}
void update_entity(EntityHandle me, float dt) {
	def_get_cmp(uc, me.deref(), Update_Component);
	if (uc)
		uc->update(me, dt);
}

// Where I'm at: Trying to get the interactions to work, but first I need box-box collision for vision boxes
// and bounding boxes. But I'm having unit errors again :( 
bool are_interacting(EntityHandle initiator, EntityHandle receiver) {
	def_get_cmp(initiator_pc, initiator.deref(), Position_Component);
	def_get_cmp(receiver_pc, receiver.deref(), Position_Component);
	def_get_cmp(vision, receiver.deref(), Vision_Component);
	def_get_cmp(ic, initiator.deref(), Interaction_Component);
	def_get_cmp(cc, receiver.deref(), Collision_Component);
	if (!(ic && cc && initiator_pc && receiver_pc)) return false;

	screen_unit centerx = 0.f;
	screen_unit centery = 0.f;
	glm::vec2 top_left = glm::vec2(centerx - .5f * vision->width, centery + .5f * vision->depth);
	glm::vec2 top_right = glm::vec2(centerx + .5f * vision->width, centery + .5f * vision->depth);
	glm::vec2 bottom_right = glm::vec2(centerx + .5f * vision->width, centery - .5f * vision->depth);
	glm::vec2 bottom_left = glm::vec2(centerx - .5f * vision->width, centery - .5f * vision->depth);
}
void debug(EntityHandle me) {
	def_get_cmp(vision, me.deref(), Vision_Component);
	def_get_cmp(pc, me.deref(), Position_Component);
	screen_unit centerx = pc->world_pos.x + .5f;
	screen_unit centery = pc->world_pos.y + .5f;
	glm::vec2 top_left = glm::vec2(centerx - .5f * vision->width, centery + .5f * vision->depth);
	glm::vec2 top_right = glm::vec2(centerx + .5f * vision->width, centery + .5f * vision->depth);
	glm::vec2 bottom_right = glm::vec2(centerx + .5f * vision->width, centery - .5f * vision->depth);
	glm::vec2 bottom_left = glm::vec2(centerx - .5f * vision->width, centery - .5f * vision->depth);
	draw_square_outline(top_left, top_right, bottom_right, bottom_left, hannah_color);
}
