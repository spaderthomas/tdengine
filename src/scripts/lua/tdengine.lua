local inspect = require('inspect')

-- Globals
Entities = {}
Components = {}

-- Callbacks
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
	  print('Tried to create an entity of type ' .. cpp_ref:get_name() .. ', but no such entity exists')
	  return
   end
   
   -- Construct the entity with a do-nothing constructor
   entity = EntityType:new()
   
   -- Inject the table with a reference back to the C++ entity
   entity.cpp_ref = cpp_ref
   entity.alive = true

   -- Store it in the global list
   Entities[cpp_ref:get_id()] = entity

   -- Call user-defined constructor
   EntityType.init(entity)
end

function on_component_created(cpp_ref)
   ComponentType = _G[cpp_ref:get_name()]
   if not ComponentType then
	  print('Tried to create an component of type ' .. cpp_ref:get_name() .. ', but no such component exists')
	  return
   end

   component = ComponentType:new()
   
   component.cpp_ref = cpp_ref
   component.alive = true
   component.parent = Entities[cpp_ref:get_entity()]

   Components[cpp_ref:get_id()] = component

   component:init()
end

function on_scene_created(cpp_ref)
  print('on_scene_created()')
  print(cpp_ref:get_name())
end


-- Class stuff
local function _create_class(name)
  local class = {
	 name = name,
	 static = {},
	 __methods = {},
  }

  -- Set up the class to look in its methods if it calls something that isn't a static method
  on_static_method_not_found = function(_, key)
	return rawget(class.__methods,key)
  end
  
  setmetatable(class.static, {
    __index = on_static_method_not_found
  })

  -- Set the class to add new fields to methods, and to check static methods before instance methods
  local metatable = {
	__index = class.static,
	__tostring = function(self) return self.name end,
	__newindex = function(class, method_name, method) class.__methods[method_name] = method end
  }
  setmetatable(class, metatable)

  return class
end

local function _includeMixin(aClass, mixin)
  assert(type(mixin) == 'table', "mixin must be a table")

  for name, method in pairs(mixin) do
    if name ~= "included" and name ~= "static" then
	  aClass[name] = method
	end
  end

  for name,method in pairs(mixin.static or {}) do
    aClass.static[name] = method
  end

  if type(mixin.included)=="function" then
	mixin:included(aClass)
  end
  return aClass
end

local class_mixin = {
  __tostring   = function(self) return 'Entity Name: ' .. tostring(self.class) end,
  
  static = {
    new = function(self, ...)
	  -- Inject the instance with basic data needed to exist as a class
	  local instance = { class = self }

	  -- Give it a 
	  local metatable = self.__methods
	  metatable.__index = function(tbl, key)
		return _G[self.name].__methods[key]
	  end
      setmetatable(instance, metatable)
	  
      return instance
    end,

    include = function(self, ...)
      for _, mixin in ipairs({...}) do _includeMixin(self, mixin) end
      return self
    end
  }
  
}

-- tdengine wrappers for sugar
function tdengine.draw_entity(entity)
  tdengine.internal.draw_entity(entity:get_id(), 0)
end


imgui.extensions = {
  PushBoolColor = function()
	imgui.PushStyleColor_2(imgui.constant.Col.Text, .9, .2, .7, 1)
  end
}

-- tdengine functions we're injecting in for sugar
local entity_mixin = {
  get_component = function(self, kind)
	local component = Entity["get_component"](self.cpp_ref, kind)
	return Components[component:get_id()]
  end,  
  add_component = function(self, kind)
	local component = Entity["add_component"](self.cpp_ref, kind)
	return Components[component:get_id()]
  end,
  get_name = function(self)
	return Entity["get_name"](self.cpp_ref)
  end,
  get_id = function(self)
	return Entity["get_id"](self.cpp_ref)
  end
}

function tdengine.entity(name)
  local class = _create_class(name)
  _includeMixin(class, class_mixin)
  class:include(entity_mixin)
  return class
end

local component_mixin = {
  get_name = function(self)
	return Component["get_name"](self.cpp_ref)
  end,
  get_id = function(self)
	return Component["get_id"](self.cpp_ref)
  end
}

function tdengine.component(name)
  local class = _create_class(name)
  _includeMixin(class, class_mixin)
  class:include(component_mixin)
  return class
end

local scene_mixin = {
  add_entity = function(self, entity)
	tdengine.add_entity_to_scene(self:get_name(), entity)
  end
}

function tdengine.scene(name)
  local class = _create_class(name)
  _includeMixin(class, class_mixin)
  class:include(entity_mixin)
  class:include(scene_mixin)
  return class
end


