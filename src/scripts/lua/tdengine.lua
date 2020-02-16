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

function on_entity_created(cpp_ref)
   -- Inject the class with a reference back to the C++ entity
   entity = {
	  cpp_ref = cpp_ref,
	  alive = true,
	  handle = handle
   }
   setmetatable(entity, entity_mt)

   -- Construct the entity
   EntityType = _G[cpp_ref:get_name()]
   EntityType.initialize(entity)
   
   Entities[cpp_ref:get_id()] = entity
end

function on_component_created(cpp_ref)
   print(cpp_ref:get_name())
end

Position = class('Position')
function Position:initialize()
   self.x = 0
   self.y = 0
end

Spader = class('Spader')
function Spader:initialize()
   print('Calling entity Lua initializer')
   self:add_component("Position")
   print(self:get_id())
   print(self:get_name())
   --local position = self:get_component("Position")
   print("Position: " .. position.x) 
end

function Spader:update(dt)
   print('updated entity, got ' .. dt .. 'for dt!')
   print(self.name)
end

SpaderComponent = class('SpaderComponent')
function SpaderComponent:initialize()
   self.cool = 400
   print('initialized spaderc with coolness=' .. self.cool)
end

function SpaderComponent:update(dt)
   print('updating spaderc, got ' .. dt .. ' for dt')
   print(self)
   print(self.fun)
end



