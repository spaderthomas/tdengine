return {
  components = {
	Graphic = {},
    Animation = {
	  animations = {
	    oliver_stand = {
		  { sprite = 'oliver_stand_001.png', time = tdengine.frames(8) },
		},
		oliver_walkdown = {
		  { sprite = 'wilson_walkdown_001.png', time = tdengine.frames(8) },
		  { sprite = 'wilson_walkdown_002.png', time = tdengine.frames(8) },
		  { sprite = 'wilson_walkdown_003.png', time = tdengine.frames(8) },
		  { sprite = 'wilson_walkdown_004.png', time = tdengine.frames(8) },
		  { sprite = 'wilson_walkdown_005.png', time = tdengine.frames(8) },
		  { sprite = 'wilson_walkdown_006.png', time = tdengine.frames(8) },
		}
	  },
	  current = 'oliver_stand'
	},
	Physics = {},
	Position = {},
	BoundingBox = {
	   extents = { x = .04, y = .03 },
	   offset = { x = 0, y = -.06 },
	},
  }
}
