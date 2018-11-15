local defaults = {
   Human_Bounding_Box = {
	  center = {
		 x = 0,
		 y = tile_y_to_screen(-.5)
	  },
	  extents = {
		 x = tile_x_to_screen(1.25),
		 y = tile_y_to_screen(.5)
	  }
   }
}

return defaults
