inspect = require('inspect')

-- Callbacks

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

function tdengine.go_to_marker(id, name)
  local marker = tdengine.markers[name]
  if not marker then
	tdengine.log('@bad_marker: ' .. name)
	return
  end

  if tdengine.loaded_scene.name ~= marker.scene then
	tdengine.change_scene(marker.scene)
  end
  tdengine.teleport_entity(id, marker.x, marker.y)
end


tdengine.story_markers = {
  INTRO               = 'intro',
  LEFT_JANITOR_CLOSET = 'left_janitor_closet'
}
