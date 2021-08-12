function tdengine.serialize_current_scene()
  local save = {
	entities = {}
  }
  
  for index, entity in pairs(tdengine.entities) do
	if entity.tdengine.should_save then
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
		if component.save and component.tdengine.should_save then
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

function tdengine.apply_overlay()
   local overlay_dir = 'scenes/overlays/' .. tdengine.loaded_scene.name .. '/'
   local overlay = overlay_dir .. tostring(tdengine.state['main:story_marker'])
   local entities = tdengine.fetch_module_data_quiet(overlay)
   if not entities then return end
   
   for index, entity in pairs(entities) do
	tdengine.create_entity(entity.name, entity)
  end
end

function tdengine.unload_current_scene()
  -- Cleanup the current scene by calling into its manager and then destroying entities
  if string.len(tdengine.loaded_scene.name) > 0 then
	local scene = tdengine.fetch_module_data('scenes/templates/' .. tdengine.loaded_scene.name)
	if scene.manager then
	  local manager = tdengine.find_entity(scene.manager)
	  assert(manager, '@no_manager_to_cleanup:' .. scene.manager)
	  manager:cleanup()
    end
  end

  for id, entity in pairs(tdengine.entities) do
	persist = entity.tdengine.persist or false
	if not persist then
	  tdengine.destroy_entity(id)
	end
  end
end

function tdengine.load_scene_template(name)  
  -- Create everything from new scene's template
  local data = tdengine.fetch_module_data('scenes/templates/' .. name)
  if not data then
	tdengine.log('@scene_load_failure:' .. name)
  end

  for index, entity in pairs(data.entities) do
	tdengine.create_entity(entity.name, entity)
  end

  tdengine.loaded_scene = {
	name = name,
	path = 'scenes/templates/' .. name,
	manager = data.manager
  }
end

function tdengine.load_scene(name)  
  tdengine.unload_current_scene()
  
  tdengine.load_scene_template(name)

  tdengine.apply_overlay()
  
  -- Set it up by calling into its manager
  local scene = tdengine.fetch_module_data('scenes/templates/' .. name)
  if scene.manager then
	local manager = tdengine.find_entity(scene.manager)
	assert(manager, '@no_manager_to_setup:' .. scene.manager)
	manager:setup()
  end
end
  
function tdengine.save(name)
  local player = tdengine.find_entity('Player')
  
  tdengine.save_current_scene_to_memory()
  local save = {
	state = tdengine.state,
	loaded_scene = tdengine.loaded_scene,
	player = player:save()
  }

  local path = tdengine.paths.absolute('src/scripts/saves/' .. name .. '.lua')
  tdengine.log('@save: ' .. path)
  tdengine.write_file_to_return_table(path, save)
end

function tdengine.load(name)
  -- Grab the save data
  local path = 'saves/' .. name
  tdengine.log('@load: ' .. path)
  package.loaded[path] = nil
  local status, state = pcall(require, path)
  if not status then
	tdengine.log('@cannot_require_state: ' .. state)
	return
  end

  -- Rehydrate the state. Do this first, so when you load the scene, you have the correct story
  -- marker (for the overlay), and the rest of the state (for any custom setup code)
  for key, value in pairs(state.state) do
	if tdengine.state[key] == nil then
	  tdengine.log('@outdated_state: ' .. key)
	else
	  tdengine.state[key] = value
	end
  end

  tdengine.load_scene(state.loaded_scene.name)

  local player = tdengine.find_entity('Player')
  tdengine.teleport_entity(player.id, state.player.position.x, state.player.position.y)
  tdengine.snap_to_player()
end

function tdengine.go_to_marker(id, name)
  local marker = tdengine.markers[name]
  if not marker then
	tdengine.log('@bad_marker: ' .. name)
	return
  end

  if tdengine.loaded_scene.name ~= marker.scene then
	tdengine.load_scene(marker.scene)
  end
  tdengine.teleport_entity(id, marker.x, marker.y)
end
