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
		boon_walkup = {
		  { sprite = 'boon_walkup_001.png', time = tdengine.frames(8) },
	      { sprite = 'boon_walkup_002.png', time = tdengine.frames(8) },
	      { sprite = 'boon_walkup_003.png', time = tdengine.frames(8) },
	      { sprite = 'boon_walkup_004.png', time = tdengine.frames(8) },
	      { sprite = 'boon_walkup_005.png', time = tdengine.frames(8) },
	      { sprite = 'boon_walkup_006.png', time = tdengine.frames(8) },
		},
		boon_surprise = {
		  { sprite = 'boon_surprise_001.png', time = tdengine.frames(20) },
	      { sprite = 'boon_surprise_002.png', time = tdengine.frames(20) },
	      { sprite = 'boon_surprise_003.png', time = tdengine.frames(20) },
	      { sprite = 'boon_surprise_004.png', time = tdengine.frames(20) },
		},
		boon_sleep = {
		  { sprite = 'boon_sleep_001.png', time = tdengine.frames(60) },
	      { sprite = 'boon_sleep_002.png', time = tdengine.frames(60) },
	      { sprite = 'boon_sleep_003.png', time = tdengine.frames(60) },
		}
	  },
	  current = 'boon_stand'
	},
	Physics = {},
	Position = {},
	BoundingBox = {
	  extents = { x = .04, y = .03 },
	  offset = { x = 0, y = -.06 },
	},
	Input = {
	  channel = tdengine.InputChannel.Player
	},
	PlayerVision = {
	  extents = { x = .03, y = .15 },
	  offset = { x = 0, y = .04 },
	}
  }
}
