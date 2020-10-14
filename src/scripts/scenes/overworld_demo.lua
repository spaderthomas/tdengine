return {
   background = '',
   entities = {
	  {
		 name = 'Background',
		 components = {
			Animation = {
			   animations = {
				  background = { 'classroom.png' }
			   },
			   current = 'background'
			}
		 }
	  },
	  {
		 name = 'Player',
		 components = {
			Position = {
			   world = { x = 0, y = 0 }
			}
		 }
	  },
	  {
		 name = 'Box',
		 components = {
			Position = {
			   world = { x = .1, y = .2 }
			}
		 }
	  }	  
   }
}
