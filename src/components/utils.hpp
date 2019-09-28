//@metaprogramming
union any_component {
	Graphic_Component graphic_component;
	Position_Component position_component;
	Movement_Component movement_component;
	Vision_Component vision;
	Interaction_Component interaction_component;
	Door_Component door_component;
	Collision_Component collision_component;
	Task_Component task_component;
	BattleComponent battle_component;
	TileComponent tile_component;
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
	{ "BattleComponent", &typeid(BattleComponent) },
	{ "TileComponent", &typeid(TileComponent) },
	// @NEXT_TYPE_INFO
};

#define def_get_cmp(var, entity, type) type* var = (entity)->get_component<type>()
#define get_cmp(entity, type) (entity)->get_component<type>()
#define def_cast_cmp(varname, cmp, type) type* varname = dynamic_cast<type*>((cmp))
#define if_component(var, entity, type) type* var = (entity)->get_component<type>(); if ((var))
