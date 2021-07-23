return {
  {
	name = 'Trigger',
	params = { on_collision = 'main:introduce_librarian' },
	components = {
	  BoundingBox = {
		extents = { x = .25, y = .25 }
	  },
	  Position = {
		world = { x = 2.24, y = 3 }
	  }
	}
  },
  {
	 name = 'StaticNpc',
	 tag = 'librarian',
	 components = {
		Animation = {
		   current = 'librarian_polish',
		   loop = true
		},
		Position = {
		   marker = 'library:introduce:librarian'
		}
	 }
  },
}
