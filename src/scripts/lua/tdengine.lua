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

function update_entity(id, dt)
   local entity = Entities[id]
   entity:update(dt)
end

function update_component(id, dt)
   local component = Components[id]
   component:update(dt)
end


local function _createIndexWrapper(aClass, f)
  if f == nil then
    return aClass.__instanceDict
  else
    return function(self, name)
      local value = aClass.__instanceDict[name]

      if value ~= nil then
        return value
      elseif type(f) == "function" then
        return (f(self, name))
      else
        return f[name]
      end
    end
  end
end

local function _declareInstanceMethod(aClass, name, f)
  aClass.__declaredMethods[name] = f
  
  f = name == "__index" and _createIndexWrapper(aClass, f) or f
  aClass.__instanceDict[name] = f
end

local function _tostring(self) return inspect(self) end
local function _call(self, ...) return self:new(...) end

local function _createClass(name)
  local dict = {}
  dict.__index = dict

  local aClass = {
	 name = name,
	 static = {},
	 __instanceDict = dict,
	 __declaredMethods = {},
  }


  -- If the method isn't static, check in the instance methods
  on_static_method_not_found = function(_, key)
	return rawget(dict,key)
  end
  
  setmetatable(aClass.static, {
    __index = on_static_method_not_found
  })

  local metatable = {
	__index = aClass.static, -- First, check in the class' static methods
	__tostring = _tostring,
	__call = _call,
	__newindex = _declareInstanceMethod
  }
  setmetatable(aClass, metatable)

  return aClass
end

local function _includeMixin(aClass, mixin)
  assert(type(mixin) == 'table', "mixin must be a table")

  for name,method in pairs(mixin) do
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

local entity_mixin = {
  __tostring   = function(self) return "instance of " .. tostring(self.class) end,

  isInstanceOf = function(self, aClass)
    return type(aClass) == 'table'
       and type(self) == 'table'
       and (self.class == aClass
            or type(self.class) == 'table'
            and type(self.class.isSubclassOf) == 'function'
            and self.class:isSubclassOf(aClass))
  end,

  -- TDAPI functions we're injecting in for sugar
  get_component = get_component,
  add_component = add_component,
  get_name = get_entity_name,
  get_id = get_entity_id,


  static = {
    allocate = function(self)
	  local metatable = self.__instanceDict
	  metatable.__index = function(tbl, key)
		return _G[self.name].__instanceDict[key]
	  end
      return setmetatable({ class = self }, metatable)
    end,

    new = function(self, ...)
      local instance = self:allocate()
      return instance
    end,

    include = function(self, ...)
      assert(type(self) == 'table', "Make sure you that you are using 'Class:include' instead of 'Class.include'")
      for _,mixin in ipairs({...}) do _includeMixin(self, mixin) end
      return self
    end
  }
}

local component_mixin = {
  __tostring   = function(self) return "instance of " .. tostring(self.class) end,

  isInstanceOf = function(self, aClass)
    return type(aClass) == 'table'
       and type(self) == 'table'
       and (self.class == aClass
            or type(self.class) == 'table'
            and type(self.class.isSubclassOf) == 'function'
            and self.class:isSubclassOf(aClass))
  end,

  -- TDAPI functions we're injecting in for sugar
  get_component_name = get_component_name,
  get_component_id = get_component_id,

  static = {
    allocate = function(self)
	  local metatable = self.__instanceDict
	  metatable.__index = function(tbl, key)
		return _G[self.name].__instanceDict[key]
	  end
      return setmetatable({ class = self }, metatable)
    end,

    new = function(self, ...)
      local instance = self:allocate()
      return instance
    end,

    include = function(self, ...)
      assert(type(self) == 'table', "Make sure you that you are using 'Class:include' instead of 'Class.include'")
      for _,mixin in ipairs({...}) do _includeMixin(self, mixin) end
      return self
    end
  }
}

function tdapi.entity(name)
  return _includeMixin(_createClass(name), entity_mixin)
end

function tdapi.component(name)
  return _includeMixin(_createClass(name), component_mixin)
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

   -- Store it in the global list
   Entities[cpp_ref:get_id()] = entity

   -- Call user-defined constructor
   EntityType.init(entity)
end

function on_component_created(cpp_ref)
  print('component created' .. cpp_ref:get_name())
   ComponentType = _G[cpp_ref:get_name()]
   if not ComponentType then
	  print('Tried to create an component of type' .. cpp_ref:get_name() .. ', but no such component exists')
	  return
   end

   component = ComponentType:new()
   
   component.cpp_ref = cpp_ref
   component.alive = true
   component.parent = Entities[cpp_ref:get_entity()]

   Components[cpp_ref:get_id()] = component

   component:init()
end


TestClass = tdapi.entity('TestClass')
function TestClass:init()
  print('TestClass:initialize(): I made it!')
end
function TestClass:test()
end
