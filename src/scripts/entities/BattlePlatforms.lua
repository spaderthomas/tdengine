BattlePlatforms = tdengine.entity('BattlePlatforms')

-- Trainer platforms
local PlatformType = {
  Player = 'player_platform',
  Opponent = 'opponent_platform'
}

local platforms = {}
platforms[PlatformType.Player] = {
  name = 'Sprite',
  tag = PlatformType.Player,
  components = {
	Position = {
	  world = { x = -.25, y = .355 }
	},
	Animation = {
	  current = 'platform_grass_player'
	},
	Slide = {
	  waypoints = { tdengine.vec2(.25, .355) },
	  times = { 1 }
	}
  }
}

platforms[PlatformType.Opponent] = {
  name = 'Sprite',
  tag = PlatformType.Opponent,
  components = {
	Position = {
	  world = { x = 1.3, y = .65 }
	},
	Animation = {
	  current = 'platform_grass_opponent'
	},
	Slide = {
	  waypoints = { tdengine.vec2(.65, .65 ) },
	  times = { 1 }
	}
  }
}

-- The sprites that are attached to the platforms (trainer, soul)
local opponent_sprite = {
  name = 'Sprite',
  tag = 'opponent',
  components = {
	Graphic = {
	  layer = 2
	},
	Animation = {
	},
	Position = {
	  tag = PlatformType.Opponent,
	  offset = { x = 0, y = .1 }
	}
  }
}

local player_sprite = {
  name = 'Sprite',
  tag = 'player',
  components = {
	Graphic = {
	  layer = 2
	},
	Animation = {
	  current = 'boon_back'
	},
	Position = {
	  tag = PlatformType.Player,
	  offset = { x = .05, y = .07825 }
	}
  }
}

function BattlePlatforms:init(params)
  self.battle = tdengine.find_entity('Battle')
  
  -- Create the platform entities. Store the slide for later so we can read / write to it.
  tdengine.create_entity('Sprite', platforms[PlatformType.Player])
  local player_platform = tdengine.find_entity_by_tag(PlatformType.Player)
  self.player_slide = player_platform:get_component('Slide')
  
  tdengine.create_entity('Sprite', platforms[PlatformType.Opponent])
  local opponent_platform = tdengine.find_entity_by_tag(PlatformType.Opponent)
  self.opponent_slide = opponent_platform:get_component('Slide')

  -- And the sprites which will be attached to the platforms
  opponent_sprite.components.Animation.current = self.battle.trainer.intro_animation
  tdengine.create_entity('Sprite', opponent_sprite)
  tdengine.create_entity('Sprite', player_sprite)
end

function BattlePlatforms:next_waypoint()
  self.player_slide:next_waypoint()
  self.opponent_slide:next_waypoint()
end

function BattlePlatforms:done()
  return self.player_slide.done and self.opponent_slide.done
end

function BattlePlatforms:update(dt)
end
