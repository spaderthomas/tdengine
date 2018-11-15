meta = {}
meta.scripts = {
   -- Order matters!
   {
	  name = "machine_conf",
	  imgui_ignore = true,
	  check_for_defs = false
   },
   {
	  name = "utils",
	  imgui_ignore = true,
	  check_for_defs = false
   },
   {
	  name = "default_components",
	  imgui_ignore = true,
	  check_for_defs = false
   },
   {
	  name = "npc",
	  imgui_ignore = false,
	  check_for_defs = true
   },
   {
	  name = "dialogue",
	  imgui_ignore = true,
	  check_for_defs = true
   },
   {
	  name = "props",
	  imgui_ignore = false,
	  check_for_defs = true
   },
   {
	  name = "tiles",
	  imgui_ignore = true,
	  check_for_defs = true
   },
   {
	  name = "boon",
	  imgui_ignore = false,
	  check_for_defs = true
   },
   {
	  name = "game",
	  imgui_ignore = true,
	  check_for_defs = false
   },
   
   -- Order doesn't matter!
   
   {
	  name = "levels",
	  imgui_ignore = true,
	  check_for_defs = false
   },
   {
	  name = "state",
	  imgui_ignore = true,
	  check_for_defs = true
   }
}
