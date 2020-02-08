print('loaded tdengine.lua!')

local class = require('middleclass')
local inspect = require('inspect')


handles = {}

local mt = {}
mt.__index = function(handle, key)
   print('hit the metatable1')
   if not handle.valid then
	  print(debug.traceback())
	  error("Error: handle is not valid!", 2)
   end

   print('hit the metatable')
   return function(handle, ...) return Entity[key](handle.cpp_ref, ...) end
end

function create_handle(cpp_ref)
   print('create_handle')
   local handle = {
	  cpp_ref = cpp_ref,
	  valid = true
   }
   setmetatable(handle, mt)

   print(inspect(handle))
   print('registered a handle in lua!')
   print(cpp_ref.id)
   handles[cpp_ref:get_id()] = handle
   return handle
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


Spader = class('Spader')
function Spader:initialize()
   print('bb init')
   self.name = "Spader"
   --add_component(self, "Graphic")
   print('initialized Spader')
end

function Spader:update(dt)
   print('updated entity, got ' .. dt .. 'for dt!')
   print(self.name)
end


