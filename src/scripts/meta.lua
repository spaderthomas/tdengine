meta = {}
meta.scripts = {
   -- Order matters!
   {
	  name = "machine_conf",
	  is_dir = false
   },
   {
	  name = "utils",
	  is_dir = false
   },
   {
	  name = "default_components",
	  is_dir = false
   },
   {
	  name = "entities",
	  imgui_ignore = false,
	  check_for_defs = true,
	  is_dir = true
   },
   {
	  name = "game",
	  is_dir = false
   }
}
