return {
  components = {
	Graphic = {},
    Animation = {
	  animations = {
	    boon_stand = {
		  'boon_stand_001.png'
		},
		boon_walkdown = {
		  'boon_walkdown_001.png',
	      'boon_walkdown_002.png',
	      'boon_walkdown_003.png',
	      'boon_walkdown_004.png',
	      'boon_walkdown_005.png',
	      'boon_walkdown_006.png'
		},
		boon_surprise = {
		  'boon_surprise_001.png',
	      'boon_surprise_002.png',
	      'boon_surprise_003.png',
	      'boon_surprise_004.png',
		}
	  },
	  current = 'boon_stand'
	},
	Physics = {},
	Position = {},
	BoundingBox = {
	   extents = { x = .05, y = .15 },
	   offset = { x = -.067, y = -.001 },
	},
	Movement = {},
	Input = {}
  }
}
