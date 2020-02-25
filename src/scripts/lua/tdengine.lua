print('Loaded tdengine.lua')

local class = require('middleclass')
local inspect = require('inspect')

Entities = {}
Components = {}

function get_component(entity, kind)
   local component = Entity["get_component"](entity.cpp_ref, kind)
   return Components[component:get_id()]
end

function add_component(entity, kind)
   local component = Entity["add_component"](entity.cpp_ref, kind)
   return Components[component:get_id()]
end

function get_entity_name(entity)
   return Entity["get_name"](entity.cpp_ref)
end

function get_entity_id(entity)
   return Entity["get_id"](entity.cpp_ref)
end

function get_component_name(component)
   return Component["get_name"](component.cpp_ref)
end

function get_component_id(component)
   return Component["get_id"](component.cpp_ref)
end

function get_parent(component)
   local id = Component["get_entity"](component.cpp_ref)
   return Entities[id]
end


-- This gets injected into user entity classes so that they have the methods of the C++ Entity class on them natively
local entity_mt = {}
entity_mt.__index = function(entity, function_name)
   return function(...)
	  if not entity.alive then
		 print(inspect(entity.handle))

		 print("You tried to call a method on an Entity that was destroyed.")
		 print(debug.traceback())
	  end

	  -- We need to pack the arguments so we can sneakily replace the component object itself with its C++ reference
	  -- for the call back to C++. Forward the rest of the arguments as usual!
	  local args_raw = {...}
	  args_raw[1] = args_raw[1].cpp_ref
	  return Entity[function_name](unpack(args_raw))
   end
end

function update_entity(id, dt)
   local entity = Entities[id]
   entity:update(dt)
end

function update_component(id, dt)
   local component = Components[id]
   component:update(dt)
end

function on_entity_created(cpp_ref)
   -- Find the matching type in Lua
   EntityType = _G[cpp_ref:get_name()]
   if not EntityType then
	  print('Tried to create an entity of type' .. cpp_ref:get_name() .. ', but no such entity exists')
	  return
   end
   
   -- Construct the entity with a do-nothing constructor
   entity = EntityType:new()
   
   -- Inject the table with a reference back to the C++ entity
   entity.cpp_ref = cpp_ref
   entity.alive = true
   entity.get_component = get_component
   entity.add_component = add_component
   entity.get_name = get_entity_name
   entity.get_id = get_entity_id

   -- Store it in the global list
   Entities[cpp_ref:get_id()] = entity

   -- Call user-defined constructor
   entity:init()
end

function on_component_created(cpp_ref)
   ComponentType = _G[cpp_ref:get_name()]
   if not ComponentType then
	  print('Tried to create an component of type' .. cpp_ref:get_name() .. ', but no such component exists')
	  return
   end

   component = ComponentType:new()
   
   component.cpp_ref = cpp_ref
   component.alive = true
   component.parent = Entities[cpp_ref:get_entity()]
   component.get_component_name = get_component_name
   component.get_component_id = get_component_id

   Components[cpp_ref:get_id()] = component

   component:init()
end

