#include "battle.hpp"
#include "collision.hpp"
#include "door.hpp"
#include "graphic.hpp"
#include "interaction.hpp"
#include "movement.hpp"
#include "position.hpp"
#include "task.hpp"
#include "tile.hpp"
#include "vision.hpp"
// @NEXT_INCLUDE

union any_component {
	Component component;
	Graphic_Component graphic_component;
	Position_Component position_component;
	Movement_Component movement_component;
	Vision_Component vision;
	Interaction_Component interaction_component;
	Door_Component door_component;
	Collision_Component collision_component;
	Task_Component task_component;
	Battle_Component battle_component;
	Tile_Component tile_component;
	// @NEXT_UNION
	
	any_component() {} // Necessary so we can in place new components in the pool.
};

Pool<any_component, DEFAULT_POOL_SIZE> component_pool;
typedef pool_handle<any_component> ComponentHandle;

unordered_map<string, const type_info*> component_map = {
	{ "Graphic_Component", &typeid(Graphic_Component) },
	{ "Position_Component", &typeid(Position_Component) },
	{ "Movement_Component", &typeid(Movement_Component) },
	{ "Vision", &typeid(Vision_Component) },
	{ "Interaction_Component", &typeid(Interaction_Component) },
	{ "Door_Component", &typeid(Door_Component) },
	{ "Task_Component", &typeid(Task_Component) },
	{ "Battle_Component", &typeid(Battle_Component) },
	{ "Tile_Component", &typeid(Tile_Component) },
	// @NEXT_TYPE_INFO
};

#define def_get_cmp(var, entity, type) type* var = (entity)->get_component<type>()
#define get_cmp(entity, type) (entity)->get_component<type>()
#define def_cast_cmp(varname, cmp, type) type* varname = dynamic_cast<type*>((cmp))
#define if_component(var, entity, type) type* var = (entity)->get_component<type>(); if ((var))

Component* Component::create(string name, TableNode* table) {
	auto handle = component_pool.next_available();
	if (name == "Graphic_Component") {
		Graphic_Component* component = &handle()->graphic_component;
		component->init(table);
			
		//@hack: I would feel better initializing this in the table?
		//       But since I want all component modifying stuff to take in an entity, I can't 
		//       directly modify the GC from within itself without duplicating the code for setting animation
		//       or doing this. Not sure?
		set_animation(entity, tds_string2(table, "default_animation"));
			
		// Set the scaling of this based on the first sprite we see. Right now, no objects resize
		// Also, use 640x360 because the raw dimensions are based on this
		Sprite* default_sprite = component->get_current_frame();
		component->scale = glm::vec2((float)default_sprite->width / (float)640,
									 (float)default_sprite->height / (float)360);
			
	}
	else if (name == "Position_Component") {
		Position_Component* component = &handle()->position_component;
		component->init(table);
	}
	else if (name == "Movement_Component") {
		Movement_Component* component = &handle()->movement_component;
		component->init(table);
		component->wish = glm::vec2(0.f);
	}
	else if (name == "Vision") {
		Vision_Component* component = &handle()->vision;
		component->init(table);
	}
	else if (name == "Interaction_Component") {
		Interaction_Component* component = &handle()->interaction_component;
		component->init(table);
	}
	else if (name == "Door_Component") {
		Door_Component* component = &handle()->door_component;
		component->init(table);
	}
	else if (name == "Collision_Component") {
		Collision_Component* component = &handle()->collision_component;
		component->init(table);
	}
	else if (name == "Task_Component") {
		Task_Component* component = &handle()->task_component;
		component->init(table);
			
		// Get the entity's current state and initialize the corresponding task
		string entity_state = tds_string(CH_STATE_KEY, entity_name);
		TableNode* task_table = tds_table(ENTITY_KEY, entity_name, SCRIPTS_KEY, entity_state);
			
		Task* task = new Task;
		task->init(task_table, entity);
		component->task = task;
	}
	else if (name == "Battle_Component") {
		Battle_Component* component = &handle()->battle_component;
		component->init(table);
	}
	else if (name == "Tile_Component") {
		Tile_Component* component = &handle()->tile_component;
		component->init(table);
	}
	// @NEXT_CREATE

	return &handle()->component;
}
