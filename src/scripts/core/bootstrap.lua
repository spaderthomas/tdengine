-- There are a lot of things we need to build up in Lua before we can start
-- loading up game data scripts, and we need to make sure things get initialized
-- in the right order.


inspect = require('inspect')

function tdengine.bootstrap()
   -- Set up globals
   tdengine.entities = {}
   tdengine.components = {}
   tdengine.actions = {}
   tdengine.loaded_scene = ''
   tdengine.active_cutscene = nil

   -- Set up console shortcuts
   tdengine.console_pipe = ''
   local console_shortcuts = {
	  scene = {
		 help = 'load a scene from a file located in src/scripts/scenes',
		 proc = tdengine.load_scene
	  },
	  cutscene = {
		 help = 'load and begin a cutscene from a file located in src/scripts/cutscenes',
		 proc = tdengine.begin_cutscene
	  },
	  ded = {
		 help = 'load a scene into the dialogue editor',
		 proc = function(name)
			local editor = tdengine.find_entity('Editor')
			editor:ded_load(name)
		 end
	  },
	  layout = {
		 help = 'use a predefined imgui layout',
		 proc = tdengine.use_layout
	  },
	  save_layout = {
		 help = 'save current imgui configuration as a layout',
		 proc = tdengine.save_layout
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
