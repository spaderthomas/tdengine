local inspect = require('inspect')

-- Utilities
tdengine.colors = {}
tdengine.colors.red =   { r = 1, g = 0, b = 0, a = 1 }
tdengine.colors.green = { r = 0, g = 1, b = 0, a = 1 }
tdengine.colors.blue =  { r = 0, g = 0, b = 1, a = 1 }


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
	  return 1
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

function on_entity_destroyed(cpp_ref)
   Entities[cpp_ref:get_id()] = nil
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

function on_component_destroyed(cpp_ref)
   Components[cpp_ref:get_id()] = nil
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

function tdengine.get_entity(name)
  for id, entity in pairs(Entities) do
	if entity:get_name() == name then
	  return entity
	end
  end

  return nil
end

-- tdengine functions we're injecting in for sugar
local entity_mixin = {
  get_component = function(self, kind)
	local component = Entity["get_component"](self.cpp_ref, kind)
	if component then
	  return Components[component:get_id()]
	else
      return nil
	end
  end,  
  add_component = function(self, kind)
	local component = Entity["add_component"](self.cpp_ref, kind)
	if component then
      return Components[component:get_id()]
	else
	  return nil
	end
  end,
  get_name = function(self)
	return Entity["get_name"](self.cpp_ref)
  end,
  get_id = function(self)
	return Entity["get_id"](self.cpp_ref)
  end,
  create_entity = function(self, name)
	 local id = tdengine.create_entity(name)
	 return Entities[id]
  end,
  destroy_entity = function(self, id)
	 tdengine.destroy_entity(id)
  end,
  get_entity = function(self, name)
	 for i=1, #Entities do
		local entity = Entities[i]
		if entity and entity:get_name() == name then return entity end
	 end
	 
	 return nil
  end,
  add_imgui_ignore = function(self, member_name)
	 self.imgui_ignore[member_name] = true
  end,
  remove_imgui_ignore = function(self, member_name)
	 self.imgui_ignore[member_name] = false
  end,
  imgui_ignore = {
	 class = true,
	 imgui_ignore = true
  }
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

local tile_mixin = {
  init = function(self)
	 local graphic = self:add_component('Graphic')
	 graphic.scale = { x = .1, y = .1 }
	 
	 local animation = self:add_component('Animation')
	 animation:add(self:get_name(), { self:get_name() .. '.png' })
	 animation:begin(self:get_name())
	 
	 local position = self:add_component('Position')
  end,
  update = function(self) end
}

function tdengine.load_scene(name)
   local scene = _G[name]
   local entities = scene.entities
   for index, entity in pairs(entities) do
	  local id = tdengine.create_entity(entity.Name)
	  if not id then return end
	  
	  local position = entity.Position
	  if position then
		 tdengine.internal.teleport_entity(id, position.x, position.y)
	  end
   end
end


-- Utilities
function tdengine.tile(name)
  local class = _create_class(name)
  _includeMixin(class, class_mixin)
  class:include(entity_mixin)
  class:include(tile_mixin)
  return class
end

function table.shallow_copy(t)
  local t2 = {}
  for k,v in pairs(t) do
    t2[k] = v
  end
  return t2
end

function tdengine.do_once(f, ...)
   if string.dump(tdengine.last_do_once) ~= string.dump(f) then
	  f(...)
	  tdengine.last_do_once = f
   end
end
tdengine.last_do_once = function() end

function tdengine.platform()
   local separator = package.config:sub(1,1)
   if separator == '\\' then
	  return 'Windows'
   elseif separator == '/' then
	  return 'Unix'
   else
	  return ''
   end
end

function tdengine.is_extension(path, extension)
   local ext_len = string.len(extension)
   local path_len = string.len(path)
   if ext_len > path_len then return false end

   local last = string.sub(path, path_len - ext_len + 1, path_len)
   return last == extension
end

function tdengine.has_extension(path)
   return string.find(path, '%.')
end

function tdengine.strip_extension(path)
   local extension = tdengine.has_extension(path)
   if not extension then return path end

   return path:sub(1, extension - 1)
end

function tdengine.scandir(dir)
   local platform = tdengine.platform()
   local command = ''
   if platform == 'Unix' then command = 'ls -a "' .. dir .. '"' end
   if platform == 'Windows' then command = 'dir "' .. dir .. '" /b /ad' end

   local i, t, popen = 0, {}, io.popen
   local pfile = popen(command)
   for filename in pfile:lines() do
	  i = i + 1
	  t[i] = filename
   end
   pfile:close()
   return t
end
