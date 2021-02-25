return {
  components = {
	Graphic = {},
    Animation = {
	  animations = {
	    boon_stand = {
		  { sprite = 'boon_stand_001.png', time = tdengine.frames(8) }
		},
		boon_walkdown = {
		  { sprite = 'boon_walkdown_001.png', time = tdengine.frames(8) },
	      { sprite = 'boon_walkdown_002.png', time = tdengine.frames(8) },
	      { sprite = 'boon_walkdown_003.png', time = tdengine.frames(8) },
	      { sprite = 'boon_walkdown_004.png', time = tdengine.frames(8) },
	      { sprite = 'boon_walkdown_005.png', time = tdengine.frames(8) },
	      { sprite = 'boon_walkdown_006.png', time = tdengine.frames(8) },
		},
		boon_surprise = {
		  { sprite = 'boon_surprise_001.png', time = tdengine.frames(8) },
	      { sprite = 'boon_surprise_002.png', time = tdengine.frames(8) },
	      { sprite = 'boon_surprise_003.png', time = tdengine.frames(8) },
	      { sprite = 'boon_surprise_004.png', time = tdengine.frames(8) },
		}
	  },
	  current = 'boon_walkdown'
	},
	Physics = {},
	Position = {},
	BoundingBox = {
	   extents = { x = .04, y = .03 },
	   offset = { x = 0, y = -.06 },
	},
	Movement = {},
	Input = {},
	PlayerVision = {
	  extents = { x = .03, y = .15 },
	  offset = { x = 0, y = .04 },
	}
  }
}
