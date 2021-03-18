inspect = require('inspect')

-- Callbacks
function update_entities(dt)
  for id, entity in pairs(tdengine.entities) do
	entity:update(dt)
  end
end

function update_components(dt)
  for id, component in pairs(tdengine.components) do
	component:update(dt)
  end
end

function update_entity(id, dt)
  local entity = tdengine.entities[id]
  if entity then
	entity:update(dt)
	return
  end
end

function update_component(id, dt)
  local component = tdengine.components[id]
  component:update(dt)
end

function on_collision(id, other)
  local entity = tdengine.entities[id]
  local other = tdengine.entities[other]

  if not entity or not other then
	print('some entity did not exist for on_collision')
  end
  
  if entity.on_collision then
	entity:on_collision(other)
  end
end

function on_interaction(id)
  local entity = tdengine.entities[id]

  if not entity then
	print('@no_entity_on_interaction, ' .. tostring(id))
  end
  
  if entity.on_interaction then
	entity:on_interaction()
  end
end

function tdengine.find_entity(name)
  for id, entity in pairs(tdengine.entities) do
	if entity:get_name() == name then
	  return entity
	end
  end

  return nil
end

function tdengine.find_entity_by_id(name)
end

function tdengine.find_entity_by_descriptor(name)
end

function tdengine.create_entity(name, data)   
  local id = tdengine.alloc_entity(name)

  -- Find the matching type in Lua
  EntityType = _G[name]
  if not EntityType then
	return nil
  end
  
  -- Construct the entity with a do-nothing constructor
  entity = EntityType:new()
  entity.id = id

  tdengine.entities[id] = entity

  -- Load the prefab to create any default compononents
  local filepath = 'prefabs/' .. name
  package.loaded[filepath] = nil
  local status, prefab = pcall(require, filepath)
  if not status then
  end

  if prefab then
	for name, component in pairs(prefab.components) do
	  local param_components = data.components or {}

	  -- Merge the component data passed in with what's in the prefab
	  local param_component = param_components[name] or {}
	  for key, value in pairs(param_component) do
		component[key] = value
	  end

	  entity:add_component(name, component)
	end
  end

  local components = entity:all_components()
  for index, component in pairs(components) do
	if component.late_init then
	  component:late_init()
	end
  end

  params = data.params or {}
  entity:init(params)

  return id
end

function tdengine.destroy_entity(id)
  local entity = tdengine.entities[id]
  entity.id = -1
  entity.alive = false
  
  tdengine.free_entity(id)
  tdengine.entities[id] = nil
end

function tdengine.create_component(entity_id, name, params)
  local id = tdengine.add_component(entity_id, name)

  ComponentType = _G[name]
  if not ComponentType then
	print('create_component(): no such component ' .. name)
	return
  end
  
  component = ComponentType:new()

  component.id = id
  component.parent = tdengine.entities[entity_id]
  
  tdengine.components[id] = component

  -- You forgot to write init() in the component. 
  if not component.init then
	local debugger = require('debugger')
	debugger.auto_where = 1
	debugger()
  end
  
  component:init(params)
end

function tdengine.destroy_component(id)
  tdengine.free_component(component);
  tdengine.components[id] = nil
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
  class.static.new = function(self, ...)
	-- Create the table we'll return to the user
	local instance = {}

	-- Give it a metatable
	local metatable = {}
	metatable.__index = function(tbl, key)
	  -- It's important that we look this up in the global namespace, because
	  -- whenever we save the file and re-script it, it will show back up there.
	  -- If we just used 'class' or 'self', that change wouldn't propagate.
	  return class_parent[self.name].__methods[key]
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
	return tdengine.has_component(self.id, kind)
  end,  
  get_component = function(self, kind)
	local id = tdengine.get_component(self.id, kind)
	if id >= 0 then
	  return tdengine.components[id]
	else
      return nil
	end
  end,  
  add_component = function(self, kind, data)
	return tdengine.create_component(self.id, kind, data)
  end,
  all_components = function(self)
	local ids = tdengine.all_components(self.id)
	local array = {}
	for index, id in pairs(ids) do
	  array[index] = tdengine.components[id]
	end
	return array
  end,
  get_name = function(self)
	local name = tdengine.entity_name(self.id)
	return name
  end,
  get_id = function(self)
	return self.id
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
  should_save = function(self, should)
	self.tdengine__should_save = should
  end,
  -- This flag gets flipped when we destroy the entity. We shouldn't be holding
  -- onto entities between frames...but sometimes we are very naughty boys indeed,
  -- and this flag will help us not explode.
  alive = true,
  tdengine__should_save = true,
  imgui_ignore = {
	class = true,
	imgui_ignore = true,
	id = true
  }
}

function tdengine.entity(name)
  local class = create_class(name)
  add_new_to_class(class, _G)
  class:include(entity_mixin)
  return class
end

local component_mixin = {
  get_name = function(self)
	return tdengine.component_name(self.id)
  end,
  get_id = function(self)
	return self.id
  end,
  do_if_parent_is = function(self, parent, f, ...)
	if self.parent:get_name() == parent then
	  f(...)
	end
  end,
  add_imgui_ignore = function(self, member_name)
	self.imgui_ignore[member_name] = true
  end,
  remove_imgui_ignore = function(self, member_name)
	self.imgui_ignore[member_name] = false
  end,
  should_save = function(self, should)
	self.tdengine__should_save = should
  end,
  tdengine__should_save = true,
  imgui_ignore = {
	class = true,
	parent = true,
	imgui_ignore = true,
	id = true
  },
}

function tdengine.component(name)
  local class = create_class(name)
  add_new_to_class(class, _G)
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
  imgui_ignore = {
	class = true,
	parent = true,
	imgui_ignore = true
  }
}

function tdengine.action(name)
  local class = create_class(name)
  add_new_to_class(class, tdengine.actions)
  class:include(action_mixin)
  return class
end

function tdengine.serialize_current_scene()
  local save = {
	entities = {}
  }

  for index, entity in pairs(tdengine.entities) do
	if entity.tdengine__should_save then
	  -- Insert some basic fields that every entity has
	  local entity_table = {
		name = entity:get_name(),
		components = {},
		params = {}
	  }

	  -- Presave: Here's where you set up your components correctly to save. E.g.
	  --   - Mark components that you don't want to save.
	  --   - Load in a temporary state to save with
	  if entity.pre_save then
		entity:pre_save()
	  end

	  -- Entity's save should return a table that are the parameters that are passed
	  -- into its init()
	  if entity.save then
		entity_table.params = entity:save()
	  end

	  -- Save out all components
	  local components = entity:all_components()
	  for index, component in pairs(components) do
		if component.save and component.tdengine__should_save then
		  entity_table.components[component:get_name()] = component:save()
		end
	  end

	  -- After all components are saved, do whatever the fuck you want
	  if entity.post_save then
		entity:post_save(entity_table)
	  end

	  table.insert(save.entities, entity_table)
	end
  end

  return save
end

function tdengine.save_current_scene_as_template(name)
  name = name or tdengine.loaded_scene.name
  local filepath = tdengine.paths.absolute('src/scripts/scenes/templates/' .. name .. '.lua')
  local data = tdengine.serialize_current_scene()
  tdengine.write_file_to_return_table(filepath, data)
end

function tdengine.save_current_scene_to_memory()
  local data = tdengine.serialize_current_scene()
  tdengine.scenes[tdengine.loaded_scene.name] = data
end


function tdengine.load_scene(data)
  for id, entity in pairs(tdengine.entities) do
	persist = entity.tdengine_persist or false
	if not persist then
	  tdengine.destroy_entity(id)
	end
  end
  
  for index, entity in pairs(data.entities) do
	tdengine.create_entity(entity.name, entity)
  end
end

function tdengine.load_scene_from_template(name)
  local module_name = 'scenes/templates/' .. name
  package.loaded[module_name] = nil
  local status, scene = pcall(require, module_name)
  if not status then
	print('tdengine.load_scene_from_template(): could not require module ' .. module_name)
	return false
  end

  tdengine.loaded_scene = {
	name = name,
	path = module_name
  }
  tdengine.load_scene(scene)
  return true
end

function tdengine.load_scene_from_memory(name)
  local scene = tdengine.scenes[name]
  if not scene then
	print('tdengine.load_scene_from_memory(): no entry for scene ' .. name)
	return false
  end
  
  tdengine.loaded_scene = {
	name = name,
	path = module_name
  }
  tdengine.load_scene(scene)
  return true
end

function tdengine.save(name)
  local player = tdengine.find_entity('Player')
  
  tdengine.save_current_scene_to_memory()
  local save = {
	state = tdengine.state,
	loaded_scene = tdengine.loaded_scene,
	scenes = tdengine.scenes,
	player = player:save()
  }

  local path = tdengine.paths.absolute('src/scripts/saves/' .. name .. '.lua')
  tdengine.log('@save: ' .. path)
  tdengine.write_file_to_return_table(path, save)
end

function tdengine.load(name)
  local path = 'saves/' .. name
  tdengine.log('@load: ' .. path)
  package.loaded[path] = nil
  local status, state = pcall(require, path)
  if not status then
	tdengine.log('@cannot_require_state: ' .. state)
	return
  end

  for key, value in pairs(state.state) do
	if tdengine.state[key] == nil then
	  tdengine.log('@outdated_state: ' .. key)
	else
	  tdengine.state[key] = value
	end
  end

  tdengine.scenes = state.scenes
  tdengine.load_scene_from_memory(state.loaded_scene.name)

  local player = tdengine.find_entity('Player')
  tdengine.teleport_entity(player.id, state.player.position.x, state.player.position.y)
  tdengine.snap_to_player()
end

function tdengine.create_action(name, params)
  ActionType = tdengine.actions[name]
  if ActionType ~= nil then
	local action = ActionType:new()

	-- Set up some default params
	action.name = name
	action.block = true
	action.done = false
	
	if contains(params, 'block') then
	  action.block = params.block
	end

	-- Run action-specific init code
	action:init(params)
	
	return action
  else
	print('create_action(): could not find action: ' .. name)
  end

  return nil
end

function tdengine.on_begin_cutscene()
  tdengine.disable_input_channel(tdengine.InputChannel.Player)
end

function tdengine.do_cutscene_from_data(data)
  tdengine.active_cutscene = {
	name = 'anonymous_cutscene',
	actions = {}
  }

  tdengine.on_begin_cutscene()

  tdengine.load_actions(data)
end

function tdengine.do_cutscene_from_name(name)
  local module_path = 'cutscenes/' .. name
  package.loaded[module_path] = nil
  local cutscene = require(module_path)

  tdengine.active_cutscene = {
	name = name,
	actions = {}
  }

  tdengine.on_begin_cutscene()

  tdengine.load_actions(cutscene)
end

function tdengine.load_actions(cutscene)
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
	tdengine.log('@finished_cutscene: ' .. tdengine.active_cutscene.name)
	tdengine.enable_input_channel(tdengine.InputChannel.Player)
	
	tdengine.active_cutscene = nil
  end
end

function tdengine.terminate_cutscene()
  tdengine.log('cutscene:@terminated: ' .. tdengine.active_cutscene.name)
  tdengine.enable_input_channel(tdengine.InputChannel.Player)
  
  tdengine.active_cutscene = nil
end

function tdengine.load_dialogue(name)
  -- Load the dialogue data itself
  local filepath = 'dialogue/' .. name
  package.loaded[filepath] = nil
  
  local status, dialogue = pcall(require, filepath)
  if not status then
	local message = 'tdengine.load_dialogue() :: could not find dialogue. '
	message = message .. 'requested dialogue was: ' .. name
	print(message)

	return nil
  end
  
  return dialogue
end

function tdengine.load_animations()
  local absolute = tdengine.paths.absolute('src/scripts/animations/')
  local animations = tdengine.scandir(absolute)
  for index, file in pairs(animations) do
	local name = string.sub(file, 1, -5)
	package.loaded[name] = nil
	local status, animation = pcall(require, name)
	
	if not status then
	  print('@cannot_require_animation: ' .. file)
	  return
	end

	tdengine.log_to('@load_animation: ' .. file, tdengine.log_flags.file)
	tdengine.animations[name] = animation
  end
end

function tdengine.load_branches()
  local absolute = tdengine.paths.absolute('src/scripts/dialogue/branches')
  local branches = tdengine.scandir(absolute)
  for index, file in pairs(branches) do
	local name = string.sub(file, 1, -5)
	local branch_module = 'dialogue/branches/' .. name
	package.loaded[branch_module] = nil
	local status, branches = pcall(require, branch_module)

	print('yep')
	if not status then
	  print('@cannot_require_branch: ' .. branches)
	  return
	end

	tdengine.log_to('@load_branch: ' .. file, tdengine.log_flags.file)

	for name, proc in pairs(branches) do
	  tdengine.branches[name] = proc
	end
  end  
end

-- Utilities
tdengine.colors = {}
tdengine.colors.red =   { r = 1, g = 0, b = 0, a = 1 }
tdengine.colors.green = { r = 0, g = 1, b = 0, a = 1 }
tdengine.colors.blue =  { r = 0, g = 0, b = 1, a = 1 }
tdengine.colors.grid_bg =  { r = .25, g = .25, b = .3, a = .8 }

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
	if filename ~= '.' and filename ~= '..' then 
	  i = i + 1
	  t[i] = filename
	end
  end
  pfile:close()
  return t
end

function tdengine.write_file_to_return_table(filepath, t)
  local file = assert(io.open(filepath, 'w'))
  if file then
	local serpent = require('serpent')
	file:write('return ')
	file:write(serpent.block(t, { comment = false }))
  else
	print('@cannot_open_file: ' .. filepath)
  end
end

function tdengine.screen_to_world(screen)
  local camera = tdengine.vec2(tdengine.camera())
  local screen = tdengine.vec2(screen)
  return screen:subtract(camera)
end		 

function tdengine.paths.script(relative)
  return tdengine.paths.absolute('src/scripts/' .. relative)
end

function tdengine.uuid()
  math.randomseed(os.time())
  local random = math.random
  local template ='xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'
  local sub = function (c)
	local v = (c == 'x') and random(0, 0xf) or random(8, 0xb)
	return string.format('%x', v)
  end
  return string.gsub(template, '[xy]', sub)
end

function tdengine.color(r, g, b, a)
  return { r = r, g = g, b = b, a = a }
end

function tdengine.color32(r, g, b, a)
  a = a * math.pow(2, 24)
  b = b * math.pow(2, 16)
  g = g * math.pow(2, 8)
  return r + g + b + a
end


local vec2_mixin = {
  unpack = function(self)
	return self.x, self.y
  end,
  add = function(self, other)
	return tdengine.vec2(self.x + other.x, self.y + other.y)
  end,
  subtract = function(self, other)
	return tdengine.vec2(self.x - other.x, self.y - other.y)
  end,
  scale = function(self, scalar)
	return tdengine.vec2(self.x * scalar, self.y * scalar)
  end,
  truncate = function(self, digits)
	return tdengine.vec2(truncate(self.x, digits), truncate(self.y, digits))
  end,
  abs = function(self)
	return tdengine.vec2(math.abs(self.x), math.abs(self.y))
  end,
  equals = function(self, other, eps)
	eps = eps or tdengine.deq_epsilon
	return double_eq(self.x, other.x, eps) and double_eq(self.y, other.y, eps)
  end
}

tdengine.vec2_impl = create_class('vec2_impl')
add_new_to_class(tdengine.vec2_impl, tdengine)
tdengine.vec2_impl:include(vec2_mixin)
tdengine.vec2 = function(x, y)
  local vec = tdengine.vec2_impl:new()

  if type(x) == 'table' then
	vec.x = x.x
	vec.y = x.y
	return vec
  else
	vec.x = x
	vec.y = y
	return vec
  end
end

function tdengine.frames(n)
  return n / 60
end

function delete(array, value)
  local len = #array
  
  for index, v in pairs(array) do
	if v == value then
	  array[index] = nil
	end
  end

  local next_available = 0
  for check = 1, len do
	if array[check] ~= nil then
	  next_available = next_available + 1
	  array[next_available] = array[check]
	end
  end

  for remove = next_available + 1, len do
	array[remove] = nil
  end
end

function contains(t, k)
  return t[k] ~= nil
end

function ternary(cond, a, b)
  if cond then return a else return b end
end

function average(a, b)
  return (a + b) / 2
end

tdengine.deq_epsilon = .00000001
function double_eq(x, y, eps)
  eps = eps or tdengine.deq_epsilon
  return math.abs(x - y) < eps
end

function is_newline(c)
  return c == '\n'
end

function is_space(c)
  return c == ' '
end

tdengine.op_or, tdengine.op_xor, tdengine.op_and = 1, 3, 4

function bitwise(oper, a, ...)
  -- base case 1: the parameter pack is empty. return nil to signal.
  if a == nil then
	return nil
  end

  local b = bitwise(oper, ...)

  -- base case 2: we're at the end of the parameter pack. just return yourself.
  if b == nil then
	return a
  end
  
  local r, m, s = 0, 2^31
  repeat
	s,a,b = a+b+m, a%m, b%m
	r,m = r + m*oper%(s-a-b), m/2
  until m < 1
  return r
end

function number_as_binary(x)
  return x
end

function truncate(float, digits)
  local mult = 10 ^ digits
  return math.modf(float * mult) / mult
end

function tdengine.layout(name)
  tdengine.push_layout(tdengine.current_layout)
  tdengine.use_layout(name)
  tdengine.current_layout = name
end

function tdengine.push_layout(name)
  delete(tdengine.layout_stack, name)
  table.insert(tdengine.layout_stack, name)
end

function tdengine.next_layout()
  local stack_size = #tdengine.layout_stack
  local index = tdengine.layout_index
  tdengine.layout_index = (index % stack_size) + 1
  
  local layout = tdengine.layout_stack[tdengine.layout_index]
  tdengine.use_layout(layout)   
end


function tdengine.previous_layout()
  local stack_size = #tdengine.layout_stack
  local index = tdengine.layout_index - 2
  tdengine.layout_index = (index % stack_size) + 1
  
  local layout = tdengine.layout_stack[tdengine.layout_index]
  tdengine.use_layout(layout)   
end

function tdengine.initialize()
  tdengine.screen('1080')
  
  -- Set up globals
  tdengine.entities = {}
  tdengine.components = {}
  tdengine.actions = {}
  tdengine.state = {}
  tdengine.branches = {}
  tdengine.story = {}
  tdengine.scenes = {}
  tdengine.animations = {}
  tdengine.triggers = {}
  tdengine.on_interactions = {}
  tdengine.loaded_scene = { name = '', path = '' }
  tdengine.active_cutscene = nil
  tdengine.locations = {}

  tdengine.current_layout = 'default'
  tdengine.layout_stack = { 'default' }
  tdengine.layout_index = 1
  
  tdengine.console_pipe = ''
  tdengine.load_console_shortcuts()

  -- Load up static data
  tdengine.load_animations()
  tdengine.load_branches()
  tdengine.load_locations()
end

function tdengine.load_console_shortcuts()
  local console_shortcuts = {
	cutscene = {
	  help = 'load + begin a cutscene from src/scripts/cutscenes',
	  proc = function(...) tdengine.do_cutscene_from_name(...) end
	},
	ded = {
	  help = 'load a scene into the dialogue editor',
	  proc = function(name)
		tdengine.layout('ded')
		local editor = tdengine.find_entity('Editor')
		editor:ded_load(name)
	  end
	},
	follow = {
	  help = 'toggle whether the camera follows the player',
	  proc = function()
		local editor = tdengine.find_entity('Editor')
		editor:toggle_follow()
	  end
	},
	layout = {
	  help = 'use a predefined imgui layout',
	  proc = tdengine.layout
	},
	q = {
	  help = 'run a script as defined in src/scripts/layouts/console',
	  proc = function(name)
		local module_path = 'layouts/console/' .. name
		package.loaded[module_path] = nil
		local status, err = pcall(require, module_path)
		if not status then
		  local message = '@quickscript_error: ' .. name
		  print(message)
		  print(err)
		end
	  end
	},
	save = {
	  help = 'save all runtime state to a file',
	  proc = function(...) tdengine.save(...) end
	},
	['load'] = {
	  help = 'load a state file',
	  proc = function(...) tdengine.load(...) end
	},
	save_layout = {
	  help = 'save current imgui configuration as a layout',
	  proc = tdengine.save_layout
	},
	scene = {
	  help = 'load a scene from src/scripts/scenes/templates',
	  proc = function(...)
		--tdengine.terminate_cutscene()
		tdengine.load_scene_from_template(...)
	  end
	},
	snap = {
	  help = 'snap the camera to center on the player',
	  proc = tdengine.snap_to_player
	},
	teleport = {
	  help = 'teleport the player + snap',
	  proc = function(x, y)
		local dest = { x = x, y = y }
		if type(x) == 'string' then
		  local location = tdengine.locations[x]
		  if not location then
			print('@bad_location: ' .. x)
			return
		  end
		  dest = location
		end

		print(inspect(dest))
		local player = tdengine.find_entity('Player')
		tdengine.teleport_entity(player.id, dest.x, dest.y)
		tdengine.snap_to_player()
	  end
	},
  }
  console_shortcuts.list = {
	help = 'list all commands and their help messages',
	proc = function()
	  local message = ''
	  for name, data in pairs(console_shortcuts) do
		message = message .. name .. ': ' .. data.help .. '\n'
	  end

	  tdengine.console_pipe = message
	end
  }
  
  for name, data in pairs(console_shortcuts) do
	_G[name] = data.proc
  end
end

function tdengine.load_locations()
  tdengine.locations = {
	library = tdengine.vec2(2.34, 2.13),
	demo = tdengine.vec2(0.07, 0.32)
  }
end

function tdengine.load_editor()
  tdengine.create_entity('Editor', {})
end

