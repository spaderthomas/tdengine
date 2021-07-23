return {
  {
	name = 'Trigger',
	params = { on_collision = 'main:introduce_hub' },
	components = {
	  BoundingBox = {
		extents = { x = .182, y = .361 }
	  },
	  Position = {
		world = { x = 2.292, y = 2.387 }
	  }
	}
  },
  {
	name = 'Eddy',
	params = {},
	components = {
	  Position = {
		marker = 'hub:intro:eddy'
	  }	  
	}
  }
}
