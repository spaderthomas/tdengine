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
  local class = tdengine.create_class(name)
  tdengine.add_new_to_class(class, _G)
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
  local class = tdengine.create_class(name)
  tdengine.add_new_to_class(class, _G)
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
   return tdengine.entities[name]
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
  entity = EntityType:new()
  entity.id = id
  entity.tag = data.tag

  tdengine.entities[id] = entity

  -- Load the prefab to create any default compononents
  local prefab = tdengine.fetch_module_data('prefabs/' .. name, false)
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
