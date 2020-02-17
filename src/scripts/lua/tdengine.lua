print('loaded tdengine.lua!')

local class = require('middleclass')
local inspect = require('inspect')

function table.pack(...)
   return {
	  ...,
	  n = select("#", ...)
   }
end

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
   print("update_entity()")
   local entity = Entities[id]
   entity:update(dt)
end

function update_component(id, dt)
   print("update_component()")
   local component = Components[id]
   component:update()
end

function on_entity_created(cpp_ref)
   -- Construct the entity with a do-nothing constructor
   EntityType = _G[cpp_ref:get_name()]
   entity = EntityType:new()
   
   -- Inject the table with a reference back to the C++ entity
   entity.cpp_ref = cpp_ref
   entity.alive = true
   entity.get_component = get_component
   entity.add_component = add_component
   entity.get_name = get_entity_name
   entity.get_id = get_entity_id
   
   -- Call user-defined constructor
   entity:init()

   -- Store it in the global list
   Entities[cpp_ref:get_id()] = entity
end

function on_component_created(cpp_ref)
   ComponentType = _G[cpp_ref:get_name()]
   component = ComponentType:new()
   
   component.cpp_ref = cpp_ref
   component.alive = true
   component.get_component_name = get_component_name
   component.get_component_id = get_component_id
   component.get_parent = get_parent

   component:init()

   Components[cpp_ref:get_id()] = component
end


Position = class('Position')
function Position:init()
   print('Position:init()')
   self.x = 0
   self.y = 0
end

function Position:update()
   print('Position:update()')
end

Spader = class('Spader')
function Spader:init()
   print('Calling Spader constructor')
   print('ID: ' .. self:get_id())
   print('Name: ' .. self:get_name())

   local position = self:add_component("Position")
   print('Position: ' .. position.x .. ', ' .. position.y)
end

function Spader:update(dt)
   print('Spader:update()')
   self:do_stuff()
end

function Spader:do_stuff()
   print('Spader::do_stuff()')
end

SpaderComponent = class('SpaderComponent')
function SpaderComponent:init()
   self.cool = 400
   print("SpaderComponent:init()")
end

function SpaderComponent:update(dt)
   print("SpaderComponent:update()")
end



