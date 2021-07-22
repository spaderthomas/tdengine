tdengine.console_shortcuts = {
  controls = {
	help = 'print basic controls',
	proc = function()
	  local message = ''
	  message = message .. 'ijkl :: move the player\n'
	  message = message .. 'wasd :: move the camera\n'
	  message = message .. 'right alt :: toggle the console\n'
	  message = message .. 'layout("default") :: use the default gui layout\n'
	  message = message .. 'ded("some_dialogue") :: load a dialogue + use dialogue editor layout\n'
	  message = message .. 'scene("some_scene") :: load a scene\n'
	  message = message .. 'teleport("some_marker") :: load the markers scene and teleport to it\n'
	  message = message .. '\nthe gui is totally resizeable. put it however the fuck\n'
	  message = message .. 'you want then call save_layout("name") to save the layout.\n'
	  message = message .. 'use layout("name") to load it later.'
	  
	  
	  tdengine.console_pipe = message

	end
  },
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
  input = {
	help = 'sometimes stuff gets stuck and player input is locked out. undo that.',
	proc = function()
	  tdengine.enable_input_channel(tdengine.InputChannel.Player)
	end
  },
  layout = {
	help = 'use a predefined imgui layout',
	proc = tdengine.layout
  },
  list = {
	help = 'list all commands and their help messages',
	proc = function()
	  local message = ''
	  for name, data in pairs(tdengine.console_shortcuts) do
		message = message .. name .. ': ' .. data.help .. '\n'
	  end
	  
	  tdengine.console_pipe = message
	end
  },
  ['load'] = {
	help = 'load a state file, as saved in scripts/save',
	proc = function(...) tdengine.load(...) end
  },
  marker = {
	help = 'add a shortcut that can be teleported to by name',
	proc = function(name)
	  local player = tdengine.find_entity('Player')
	  local position = player:get_component('Position')
	  local marker = {
		scene = tdengine.loaded_scene.name,
		x = position.world.x,
		y = position.world.y,
	  }
	  tdengine.markers[name] = marker

	  local message = '@add_marker: ' .. name .. '(' .. tostring(position.world.x) .. ', '
	  message = message .. tostring(position.world.y) .. ')'
	  tdengine.log(message)

	  local marker_filepath = tdengine.paths.absolute('src/scripts/scenes/markers.lua')
	  tdengine.write_file_to_return_table(marker_filepath, tdengine.markers)
	end
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
  reload = {
	help = 'reload current scene + overlay',
	proc = function()
	  tdengine.load_scene(tdengine.loaded_scene.name)
	end
  },
  save = {
	help = 'save all runtime state to a file',
	proc = function(...) tdengine.save(...) end
  },
  save_layout = {
	help = 'save current imgui configuration as a layout',
	proc = tdengine.save_layout
  },
  scene = {
	help = 'load a scene + overlay',
	proc = function(name)
	  tdengine.load_scene(name)
	end
  },
  snap = {
	help = 'snap the camera to center on the player',
	proc = tdengine.snap_to_player
  },
  story = {
	help = 'set the story marker',
	proc = function(story_marker)
	  tdengine.state['main:story_marker'] = tdengine.story_markers[story_marker]
	end
  },
  teleport = {
	help = 'teleport the player + snap',
	proc = function(x, y)
	  local player = tdengine.find_entity('Player')
	  if type(x) == 'string' then
		local marker = x
		tdengine.go_to_marker(player.id, marker)
	  else
		tdengine.teleport_entity(player.id, x, y)
	  end
	  
	  tdengine.snap_to_player()
	end
  }
}

-- When we type stuff in the editor's console, it's really annoying to preface
-- everything with 'tdengine.', so just stick these few useful functions in the
-- global namespace
for name, data in pairs(tdengine.console_shortcuts) do
  _G[name] = data.proc
end
