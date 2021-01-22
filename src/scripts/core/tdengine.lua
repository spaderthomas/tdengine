-- Callbacks
function update_entity(id, dt)
  local entity = tdengine.entities[id]
  entity:update(dt)
end

function update_component(id, dt)
   local component = tdengine.components[id]
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
   entity.cpp_ref = cpp_ref
   
   tdengine.entities[cpp_ref:get_id()] = entity

   tdengine.load_prefab(entity)

   EntityType.init(entity)
end

function on_entity_destroyed(cpp_ref)
   tdengine.entities[cpp_ref:get_id()] = nil
end

function on_component_created(cpp_ref, data)
   ComponentType = _G[cpp_ref:get_name()]
   if not ComponentType then
	  print('Tried to create an component of type ' .. cpp_ref:get_name() .. ', but no such component exists')
	  return
   end
   
   component = ComponentType:new()

   component.cpp_ref = cpp_ref
   component.parent = tdengine.entities[cpp_ref:get_entity()]
   
   tdengine.components[cpp_ref:get_id()] = component

   component:load(data)

   -- When we create entities from prefabs, we want to make sure everything
   -- is created and loaded before we run init (which may make API calls that
   -- require other components to be there).
   --
   -- When we directly call add_component() at runtime, we want to initialize
   -- it straight off.
   local skip_init = data.skip_init or false
   if not skip_init then
      component:init()
   end
end

function on_component_destroyed(cpp_ref)
   tdengine.components[cpp_ref:get_id()] = nil
end

-- Class stuff
local function include_mixin(class, mixin)
  assert(type(mixin) == 'table', "mixin must be a table")

  for name, method in pairs(mixin) do
    if name ~= "static" then
	  class[name] = method
	end
  end

  for name,method in pairs(mixin.static or {}) do
    class.static[name] = method
  end

  return class
end

-- Goal: When you update a method and save it, make it so that re-scripting the file
-- will automatically update the method in all instances.
-- To do this, we need to have __index route all method calls to the most current
-- version of the class. That could be in a couple of places, depending on the class
-- type: _G for entities and components, and tdengine.actions for actions.
local function add_new_to_class(class, class_parent)
   local parent = {}
   if class_parent == 'Entity' then
	  parent = _G
   end
   if class_parent == 'Component' then
	  parent = _G
   end
   if class_parent == 'Action' then
	  parent = tdengine.actions
   end
  class.static.new = function(self, ...)
	  -- Create the table we'll return to the user
	  local instance = {}

	  -- Give it a metatable
	  local metatable = {}
	  metatable.__index = function(tbl, key)
		 -- It's important that we look this up in the global namespace, because
		 -- whenever we save the file and re-script it, it will show back up there.
		 -- If we just used 'class' or 'self', that change wouldn't propagate.
		 return parent[self.name].__methods[key]
	  end
      setmetatable(instance, metatable)

      return instance
    end
end

local function create_class(name)
  local class = {
	 name = name,
	 static = {},
	 __methods = {},
  }

  -- Allow the class to include mixins. Do not use 'class' as the parameter, because
  -- this will be called like ClassName:include(), and could be called multiple
  -- times
  class.static.include = function(class_table, ...)
	 for _, mixin in ipairs({...}) do include_mixin(class_table, mixin) end
	 return self
  end

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
	__newindex = function(class, method_name, method)
	   class.__methods[method_name] = method
	end
  }
  setmetatable(class, metatable)

  return class
end

-- tdengine functions we're injecting in for sugar
local entity_mixin = {
  has_component = function(self, kind)
	return Entity["has_component"](self.cpp_ref, kind)
  end,  
  get_component = function(self, kind)
	local component = Entity["get_component"](self.cpp_ref, kind)
	if component then
	  return tdengine.components[component:get_id()]
	else
      return nil
	end
  end,  
  add_component = function(self, kind, data)
	local component = Entity["add_component"](self.cpp_ref, kind, data)
	if component then
      return tdengine.components[component:get_id()]
	else
	  return nil
	end
  end,
  all_components = function(self)
    local components = Entity["all_components"](self.cpp_ref)
	local array = {}
	for i = 1, #components do
	  local component = components[i]
	  array[i] = tdengine.components[component:get_id()]
	end
	return array
  end,
  get_name = function(self)
	return Entity["get_name"](self.cpp_ref)
  end,
  get_id = function(self)
	return Entity["get_id"](self.cpp_ref)
  end,
  create_entity = function(self, name)
	 local id = tdengine.create_entity(name)
	 return tdengine.entities[id]
  end,
  destroy_entity = function(self, id)
	 tdengine.destroy_entity(id)
  end,
  add_imgui_ignore = function(self, member_name)
	 self.imgui_ignore[member_name] = true
  end,
  remove_imgui_ignore = function(self, member_name)
	 self.imgui_ignore[member_name] = false
  end,
  -- Do not destroy this thing when you load a new scene
  persist = function(self)
	 self.tdengine_persist = true
  end,
  do_not_save = function(self)
	 self.tdengine_do_not_save = true
  end,
  imgui_ignore = {
	 class = true,
	 imgui_ignore = true,
	 tdengine_persist = true,
	 tdengine_do_not_save = true
  }
}

function tdengine.entity(name)
  local class = create_class(name)
  add_new_to_class(class, 'Entity')
  class:include(entity_mixin)
  return class
end

local component_mixin = {
  get_name = function(self)
	return Component["get_name"](self.cpp_ref)
  end,
  get_id = function(self)
	return Component["get_id"](self.cpp_ref)
  end,
  add_imgui_ignore = function(self, member_name)
	 self.imgui_ignore[member_name] = true
  end,
  remove_imgui_ignore = function(self, member_name)
	 self.imgui_ignore[member_name] = false
  end,
  imgui_ignore = {
	 class = true,
	 parent = true,
	 imgui_ignore = true
  }
}

function tdengine.component(name)
  local class = create_class(name)
  add_new_to_class(class, 'Component')
  class:include(component_mixin)
  return class
end

local action_mixin = {
  add_imgui_ignore = function(self, member_name)
	 self.imgui_ignore[member_name] = true
  end,
  remove_imgui_ignore = function(self, member_name)
	 self.imgui_ignore[member_name] = false
  end,
  block = true,
  done = false,
  imgui_ignore = {
	 class = true,
	 parent = true,
	 imgui_ignore = true
  }
}

function tdengine.action(name)
   local class = create_class(name)
   add_new_to_class(class, 'Action')
   class:include(action_mixin)
   return class
end

function tdengine.load_scene(name)
   package.loaded['scenes/'..name] = nil
   local scene = require('scenes/' .. name)
   
   for id, entity in pairs(tdengine.entities) do
	  persist = entity.tdengine_persist or false
	  if not persist then
		 tdengine.destroy_entity(id)
	  end
   end
  
   for index, data in pairs(scene.entities) do
	  local id = tdengine.create_entity(data.name)
	  if not id then return end

	  local entity = tdengine.entities[id]
	  tdengine.load_entity(entity, data)
   end

   tdengine.loaded_scene = name
end

function tdengine.load_entity(entity, data)
   local components = data.components
   if not components then return end

   for name, data in pairs(components) do
	  if entity:has_component(name) then
		 local component = entity:get_component(name)
		 component:load(data)
	  else
		 data.skip_init = true
		 local component = entity:add_component(name, data)		 
	  end
   end
   
   for name, data in pairs(components) do
	  local component = entity:get_component(name)
	  component:init()
   end

   entity:init()
end

function tdengine.load_prefab(entity)
   local data = require('prefabs/' .. entity:get_name())

   local components = data.components
   if not components then return end

   for name, data in pairs(components) do
	  if entity:has_component(name) then
		 local component = entity:get_component(name)
		 component:load(data)
	  else
		 data.skip_init = true
		 local component = entity:add_component(name, data)		 
	  end
   end
   
   for name, data in pairs(components) do
	  local component = entity:get_component(name)
	  component:init()
   end
end

function tdengine.save_scene(name)
   local relative = 'src/scripts/scenes/' .. name .. '.lua'
   local filepath = tdengine.paths.absolute_path(relative)
   local file = assert(io.open(filepath, 'w'))
   if file then
	  local scene = require('scenes/' .. name)
	  local save = {
		 entities = {}
	  }

	  for index, entity in pairs(tdengine.entities) do
		 do_not_save = entity.tdengine_do_not_save or false
		 if not do_not_save then
			-- Insert some basic fields that every entity has
			local saved = {
			   name = entity:get_name(),
			   components = {}
			}

			-- If the entity has some custom save logic, call it first. It will
			-- return a table which we merge into the main table for this entity
			if entity.save then
			   local extra = entity:save()
			   for key, value in pairs(extra) do
				  saved[key] = value
			   end
			end

			-- Save out all components
			local components = entity:all_components()
			for index, component in pairs(components) do
			   if component.save then
				  saved.components[component:get_name()] = component:save()
			   end
			end

			table.insert(save.entities, saved)
		 end
	  end

	  local serpent = require('serpent')
	  print(serpent.block(save, { comment = false }))
	  file:write('return ')
	  file:write(serpent.block(save, { comment = false }))
   end
end

-- Utilities
tdengine.colors = {}
tdengine.colors.red =   { r = 1, g = 0, b = 0, a = 1 }
tdengine.colors.green = { r = 0, g = 1, b = 0, a = 1 }
tdengine.colors.blue =  { r = 0, g = 0, b = 1, a = 1 }

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

function tdengine.screen_to_world(screen)
   return {
      x = screen.x - tdengine.get_camera_x(),
      y = screen.y - tdengine.get_camera_y()
   }
end		 

function tdengine.ray_cast(x, y)
   local id = tdengine.internal.ray_cast(x, y)
   return tdengine.entities[id]
end

function tdengine.cursor()
   return {
	  x = tdengine.get_cursor_x(),
	  y = tdengine.get_cursor_y()
   }
end

function tdengine.create_action(name, params)
   ActionType = tdengine.actions[name]
   if ActionType ~= nil then
	  local action = ActionType:new()
	  action:init(params)
	  return action
   else
	  print('create_action(): could not find action: ' .. name)
   end

   return nil
end

function tdengine.begin_cutscene(name)
   local module_path = 'cutscenes/' .. name
   package.loaded[module_path] = nil
   local cutscene = require(module_path)

   tdengine.active_cutscene = {
	  name = name,
	  actions = {}
   }
   for index, data in pairs(cutscene) do
	  local action = tdengine.create_action(data.name, data)
	  tdengine.active_cutscene.actions[index] = action
   end
end

function tdengine.update_actions(dt, actions)
   local done = true
   for index, action in pairs(actions) do
	  if not action.done then
		 action:update(dt)
		 done = false
		 
		 if action.block then
			break
		 end
	  end
   end

   return done
end

function tdengine.update_cutscene(dt)
   if tdengine.active_cutscene == nil then
	  return
   end

   local done = tdengine.update_actions(dt, tdengine.active_cutscene.actions)
   if done then
	  print('update_cutscene(): finished ' .. tdengine.active_cutscene.name)
	  tdengine.active_cutscene = nil
   end
end
