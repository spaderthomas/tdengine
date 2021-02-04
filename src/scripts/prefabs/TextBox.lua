return {
   components = {
	  Graphic = {
		 layer = 10,
		 flags = tdengine.render_flags.screen_position
	  },
	  Animation = {
		 animations = {
			text_box = {
			   'text_box.png'
			}
		 },
		 current = 'text_box'
	  },
	  Position = {
		 world = {
            x = .5,
            y = .175
		 }
	  }
   }
}
