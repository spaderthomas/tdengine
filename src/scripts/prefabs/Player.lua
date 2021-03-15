return {
  components = {
	Graphic = {},
    Animation = {
	  current = 'red_stand'
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
