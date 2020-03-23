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

union Component_Union {
	Battle_Component battle_component;
	Collision_Component collision_component;
	Door_Component door_component;
	Graphic_Component graphic_component;
	Interaction_Component interaction_component;
	Movement_Component movement_component;
	Position_Component position_component;
	Task_Component task_component;
	Tile_Component tile_component;
	Vision_Component vision;
	// @NEXT_UNION

	// You need to have a constructor so you can in place new these when you dish out pool slots
	Component_Union() {}
};

typedef pool_handle<Component_Union> ComponentHandle;
Pool<Component_Union, DEFAULT_POOL_SIZE> component_pool;

std::unordered_map<std::string, const std::type_info*> component_map = {
	{ "Battle_Component", &typeid(Battle_Component) },
	{ "Collision_Component", &typeid(Collision_Component) },
	{ "Door_Component", &typeid(Door_Component) },
	{ "Graphic_Component", &typeid(Graphic_Component) },
	{ "Interaction_Component", &typeid(Interaction_Component) },
	{ "Position_Component", &typeid(Position_Component) },
	{ "Movement_Component", &typeid(Movement_Component) },
	{ "Task_Component", &typeid(Task_Component) },
	{ "Tile_Component", &typeid(Tile_Component) },
	{ "Vision_Component", &typeid(Vision_Component) },
	// @NEXT_TYPE_INFO
};


#define def_get_cmp(var, entity, type) type* var = (entity)->get_component<type>()
#define get_cmp(entity, type) (entity)->get_component<type>()
#define def_cast_cmp(varname, cmp, type) type* varname = dynamic_cast<type*>((cmp))
#define if_component(var, entity, type) type* var = (entity)->get_component<type>(); if ((var))

template<typename ComponentType>
Component* create_component2(TableNode* table) {
	static_assert(std::is_base_of_v<Component, ComponentType>);
	
	auto handle = component_pool.next_available();
	new(handle()) ComponentType;
	ComponentType* component = (ComponentType*)handle();
	component->init(table);
	return component;
}

typedef Component* (*ComponentCreator)(TableNode*);
typedef std::map<std::string, ComponentCreator> ComponentRegistry;
ComponentRegistry component_registry;

template<typename ComponentType>
struct ComponentRegistryEntry {
	static const ComponentRegistryEntry<ComponentType>& instance(std::string type) {
		static const ComponentRegistryEntry<ComponentType>& the_instance(type);
		return the_instance;
	}

	ComponentRegistryEntry(std::string type) {
		component_registry[type] = create_component2<ComponentType>;
	}
};

#define REGISTER_COMPONENT(ComponentType)                      \
template<typename T>                                           \
struct ComponentRegistrar;                                     \
                                                               \
template<>                                                     \
struct ComponentRegistrar<ComponentType> {                     \
	static const ComponentRegistryEntry<ComponentType>& entry; \
};                                                             \
                                                               \
const ComponentRegistryEntry<ComponentType>& ComponentRegistrar<ComponentType>::entry = ComponentRegistryEntry<ComponentType>::instance(#ComponentType); 

REGISTER_COMPONENT(Tile_Component);
REGISTER_COMPONENT(Graphic_Component);
REGISTER_COMPONENT(Position_Component);
REGISTER_COMPONENT(Movement_Component);
REGISTER_COMPONENT(Vision_Component);

Component* create_component_from_string(std::string type, TableNode* table) {
	auto creator = component_registry[type];
	return creator(table);
}

ComponentHandle create_component(std::string name, TableNode* table) {
	auto handle = component_pool.next_available();
	if (name == "Graphic_Component") {
		new(handle()) Graphic_Component;
		Graphic_Component* component = (Graphic_Component*)handle();
		component->init(table);
	}
	else if (name == "Position_Component") {
		new(handle()) Position_Component;
		Position_Component* component = (Position_Component*)handle();
		component->init(table);
	}
	else if (name == "Movement_Component") {
		new(handle()) Movement_Component;
		Movement_Component* component = (Movement_Component*)handle();
		component->init(table);
	}
	else if (name == "Vision_Component") {
		new(handle()) Vision_Component;
		Vision_Component* component = (Vision_Component*)handle();
		component->init(table);
	}
	else if (name == "Interaction_Component") {
		new(handle()) Interaction_Component;
		Interaction_Component* component = (Interaction_Component*)handle();
		component->init(table);
	}
	else if (name == "Door_Component") {
		new(handle()) Door_Component;
		Door_Component* component = (Door_Component*)handle();
		component->init(table);
	}
	else if (name == "Collision_Component") {
		new(handle()) Collision_Component;
		Collision_Component* component = (Collision_Component*)handle();
		component->init(table);
	}
	else if (name == "Task_Component") {
		new(handle()) Task_Component;
		Task_Component* component = (Task_Component*)handle();
		component->init(table);
	}
	else if (name == "Battle_Component") {
		new(handle()) Battle_Component;
		Battle_Component* component = (Battle_Component*)handle();
		component->init(table);
	}
	else if (name == "Tile_Component") {
		new(handle()) Tile_Component;
		Tile_Component* component = (Tile_Component*)handle();
		component->init(table);
	}
	// @NEXT_CREATE

	return handle;
}

template<typename ComponentType>
ComponentType* component_cast(ComponentHandle handle) {
	static_assert(std::is_base_of_v<Component, ComponentType>,
				  "You tried to cast a component handle into a type that's not derived from a component.");
	return (ComponentType*)handle();
}

struct TemplateComponents {
	std::map<std::string, ComponentHandle> templates;

	Component* operator[](std::string which) {
		return (Component*)templates[which]();
	}
	
	void init() {
		// Clear out the old ones we had
		for (auto& [type, handle] : templates) {
			if (handle) {
				component_pool.mark_available(handle);
			}
		}

		// Make new ones
		auto types = all_component_types();
		for(auto& type : types) {
			TableNode* template_table = tds_table("templates", type);
			templates[type] = create_component(type, template_table);
		}
	}
};
TemplateComponents template_components;
