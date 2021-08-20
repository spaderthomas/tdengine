-- tdengine functions we're injecting in for sugar -- only functions can go here. check out how
-- the class stuff works for more info, anything put here is shared by all instances of the class.
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
	self.tdengine.persist = true
  end,
  should_save = function(self, should)
	self.tdengine.should_save = should
  end
}

function tdengine.entity(name)
  local class = tdengine.create_class(name)
  tdengine.add_new_to_class(class, _G)
  class:include(entity_mixin)
  
  tdengine.entity_types[name] = class

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
	self.tdengine.should_save = should
  end
}

function tdengine.component(name)
  local class = tdengine.create_class(name)
  tdengine.add_new_to_class(class, _G)
  class:include(component_mixin)

  tdengine.component_types[name] = class

  return class
end

local action_mixin = {
  add_imgui_ignore = function(self, member_name)
	self.imgui_ignore[member_name] = true
  end,
  remove_imgui_ignore = function(self, member_name)
	self.imgui_ignore[member_name] = false
  end,
  initted = false,
  imgui_ignore = {
	class = true,
	parent = true,
	imgui_ignore = true
  }
}

function tdengine.find_entity(name)
  for id, entity in pairs(tdengine.entities) do
	if entity:get_name() == name then
	  return entity
	end
  end

  return nil
end

function tdengine.find_entity_by_id(id)
   return tdengine.entities[id]
end

function tdengine.find_entity_by_tag(tag)
  for id, entity in pairs(tdengine.entities) do
	if entity.tag == tag then
	  return entity
	end
  end

  return nil
end

function tdengine.find_entity_by_any(descriptor)
   if descriptor.id then
	  return tdengine.find_entity_by_id(descriptor.id)
   elseif descriptor.tag then
	  return tdengine.find_entity_by_tag(descriptor.tag)
   elseif descriptor.entity then
	  return tdengine.find_entity(descriptor.entity)
   else
	  tdengine.log('@no_descriptor_to_find')
	  return nil
   end
end

function tdengine.create_entity(name, data)   
  local id = tdengine.alloc_entity(name)

  -- Find the matching type in Lua
  EntityType = _G[name]
  if not EntityType then
	return nil
  end
  
  -- Construct the entity with a do-nothing constructor
  local entity = EntityType:new()

  -- Fill in its identifiers
  entity.id = id
  entity.tag = data.tag

  -- Fill in some internal tdengine data
  entity.tdengine = {
	alive = true,
	persist = false,
	should_save = true
  }
  entity.imgui_ignore = {
	class = true,
	imgui_ignore = true
  }
  
  tdengine.entities[id] = entity

  local merge_component_data = function(prefab, param_data)
	-- No parameters passed into the component means nothing to merge.
	if not param_data then return end

	-- Go through each component kind that has extra parameters and either:
	for component_name, component_data in pairs(param_data) do

	  -- Make them ALL the params if the prefab doesn't have this component
	  if not prefab[component_name] then prefab[component_name] = {} end
	  

	  -- Add or override entries from the prefab
	  local prefab_data = prefab[component_name]
	  for key, value in pairs(component_data) do
		prefab_data[key] = value
	  end
	end
  end
  
  -- Load the prefab to create any default compononents
  local prefab = tdengine.fetch_module_data_quiet('prefabs/' .. name)
  if prefab then
	merge_component_data(prefab.components, data.components)
	for name, component in pairs(prefab.components) do
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

  -- @dev @spader 3/28/2021
  -- For now, we keep track of all the entities we created during a cutscene
  -- so the editor can clean them up if we terminate the cutscene.
  if tdengine.active_cutscene then
	 table.insert(tdengine.entities_created_during_cutscene, id)
  end
  
  return id
end

function tdengine.destroy_entity(id)
  local entity = tdengine.entities[id]
  if not entity then return end

  tdengine.free_entity(id)
  tdengine.entities[id] = nil
  entity.tdengine.alive = false
end

function tdengine.create_component(entity_id, name, params)
  local id = tdengine.add_component(entity_id, name)

  ComponentType = _G[name]
  if not ComponentType then
	tdengine.log('create_component(): no such component ' .. name)
	return
  end
  
  component = ComponentType:new()

  -- Add identifiers
  component.id = id
  component.parent = tdengine.entities[entity_id]

  -- Add some engine internal stuff
  component.tdengine = {
	should_save = true
  }
  component.imgui_ignore = {
	class = true,
	parent = true,
	imgui_ignore = true,
	id = true
  }
  
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
