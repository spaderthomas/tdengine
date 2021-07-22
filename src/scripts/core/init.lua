function tdengine.load_editor()
  tdengine.create_entity('Editor', {})
end

function tdengine.load_animations()
  local absolute = tdengine.paths.absolute('src/scripts/animations/')
  local animations = tdengine.scandir(absolute)
  for index, file in pairs(animations) do
	local name = string.sub(file, 1, -5)
	package.loaded[name] = nil
	local status, animation = pcall(require, name)
	
	if not status then
	  tdengine.log('@cannot_require_animation: ' .. file)
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

function tdengine.load_markers()
  local module_path = 'scenes/markers'
  package.loaded[module_path] = nil
  local status, markers = pcall(require, module_path)
  if not status then
	local message = '@error_loading_markers: '
	print(message)
	print(markers)
  end

  tdengine.markers = markers
end

function tdengine.initialize()
  tdengine.screen('1080')
  
  -- Set up globals
  tdengine.entities = {}
  tdengine.components = {}
  tdengine.actions = {}
  tdengine.state = {}
  tdengine.markers = {}
  tdengine.scenes = {}
  tdengine.animations = {}
  tdengine.branches = {}
  tdengine.triggers = {}
  tdengine.on_interactions = {}
  tdengine.loaded_scene = { name = '', path = '' }
  tdengine.active_cutscene = nil
  tdengine.entities_created_during_cutscene = {}

  tdengine.current_layout = 'default'
  tdengine.layout_stack = { 'default' }
  tdengine.layout_index = 1
  
  tdengine.console_pipe = ''

  -- Load up static data
  tdengine.load_animations()
  tdengine.load_branches()
  tdengine.load_markers()
  tdengine.load_default_state()
end
