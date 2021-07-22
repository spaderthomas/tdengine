function tdengine.action(name)
  local class = tdengine.create_class(name)
  tdengine.add_new_to_class(class, tdengine.actions)
  class:include(action_mixin)
  return class
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

	-- action:init() is NOT run here -- we almost always want to init
	-- the action right before it runs!
	action.params = params
	
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
	  -- Initialize
	  if not action.initted then
		local params = action.params
		action:init(params)
		action.initted = true
	  end

	  -- An action can mark itself as done during init. Usually this is an error
	  -- condition. Forgo the action if this is the case so we don't have to pollute
	  -- the action code with this check in update()
	  if not action.done then
		 action:update(dt)
		 done = false
	  end

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
  if not tdengine.active_cutscene then return end
  
  tdengine.log('@terminated_cutscene: ' .. tdengine.active_cutscene.name)
  tdengine.enable_input_channel(tdengine.InputChannel.Player)

  for index, id in pairs(tdengine.entities_created_during_cutscene) do
	 tdengine.destroy_entity(id)
  end
  tdengine.entities_created_during_cutscene = {}
  
  tdengine.active_cutscene = nil
end
