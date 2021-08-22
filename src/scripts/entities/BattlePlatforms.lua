BattlePlatforms = tdengine.entity('BattlePlatforms')

-- Trainer platforms
BattlePlatforms.State = {
  Idle = 'Idle',
  Sliding = 'Sliding',
  FinishedSliding = 'FinishedSliding',
  AnimatingLead = 'AnimatingLead'
}

BattlePlatforms.PlatformType = {
  Player = 'player_platform',
  Opponent = 'opponent_platform'
}

local platforms = {}
platforms[BattlePlatforms.PlatformType.Player] = {
  name = 'Sprite',
  tag = BattlePlatforms.PlatformType.Player,
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

platforms[BattlePlatforms.PlatformType.Opponent] = {
  name = 'Sprite',
  tag = BattlePlatforms.PlatformType.Opponent,
  components = {
	Position = {
	  world = { x = 1.3, y = .65 }
	},
	Animation = {
	  current = 'platform_grass_opponent'
	},
	Slide = {
	  waypoints = { tdengine.vec2(.65, .65 ) },
	  times = { 1 },
	}
  }
}

-- The sprites that are attached to the platforms (trainer, soul)
local opponent_sprite = {
  name = 'Sprite',
  tag = 'opponent_sprite',
  components = {
	Graphic = {
	  layer = 2
	},
	Animation = {
	},
	Position = {
	  tag = BattlePlatforms.PlatformType.Opponent,
	  offset = { x = 0, y = .1 }
	},
	Slide = {
	  waypoints = { },
	  times = { },
	  loop = true
	}
  }
}

local player_sprite = {
  name = 'Sprite',
  tag = 'player_sprite',
  components = {
	Graphic = {
	  layer = 2
	},
	Animation = {
	  current = 'boon_back'
	},
	Position = {
	  tag = BattlePlatforms.PlatformType.Player,
	  offset = { x = .05, y = .07825 }
	},
	Slide = {
	  waypoints = { },
	  times = { },
	  loop = true
	}
  }
}

function BattlePlatforms:init(params)
  self.state = BattlePlatforms.State.Idle
  
  self.battle = tdengine.find_entity('Battle')
  
  -- Create the platform entities. Store the slide for later so we can read / write to it.
  tdengine.create_entity('Sprite', platforms[BattlePlatforms.PlatformType.Player])
  local player_platform = tdengine.find_entity_by_tag(BattlePlatforms.PlatformType.Player)
  self.player_platform = player_platform
  self.player_platform_slide = player_platform:get_component('Slide')
  
  tdengine.create_entity('Sprite', platforms[BattlePlatforms.PlatformType.Opponent])
  local opponent_platform = tdengine.find_entity_by_tag(BattlePlatforms.PlatformType.Opponent)
  self.opponent_platform = opponent_platform
  self.opponent_platform_slide = opponent_platform:get_component('Slide')

  -- And the sprites which will be attached to the platforms
  opponent_sprite.components.Animation.current = self.battle.trainer.intro_animation
  local id = tdengine.create_entity('Sprite', opponent_sprite)
  self.opponent_sprite = tdengine.find_entity_by_id(id)
  self.opponent_sprite_slide = self.opponent_sprite:get_component('Slide')
  local id = tdengine.create_entity('Sprite', player_sprite)
  self.player_sprite = tdengine.find_entity_by_id(id)
  self.player_sprite_slide = self.player_sprite:get_component('Slide')
end

function BattlePlatforms:update(dt)
  if self.state == BattlePlatforms.State.Sliding then
	local done = true
	done = done and self.player_platform_slide.done
	done = done and self.opponent_platform_slide.done
	done = done and self.player_sprite_slide.done
	done = done and self.opponent_sprite_slide.done
	if done then
	  self.state = BattlePlatforms.State.FinishedSliding
	end
  end
end

function BattlePlatforms:done()
  local done = false
  done = done or self.state == BattlePlatforms.State.FinishedSliding
  done = done or self.state == BattlePlatforms.State.Idle
  return done
end

function BattlePlatforms:next_waypoint()
  self.player_platform_slide:next_waypoint()
  self.opponent_platform_slide:next_waypoint()
  self.state = BattlePlatforms.State.Sliding
end

function BattlePlatforms:slide_out_sprite(sprite, target)
  tdengine.detach_position(sprite.id)
  local slide = sprite:get_component('Slide')
  local waypoints = { target }
  local times = { 1 }
  slide:set_waypoints(waypoints, times)
  slide:next_waypoint()

  self.state = BattlePlatforms.State.Sliding
end

function BattlePlatforms:slide_out_player_sprite()
  local sprite = self.player_sprite
  local position = sprite:get_component('Position')
  local target = { x = -.2, y = position.world.y }
  self:slide_out_sprite(sprite, target)
end

function BattlePlatforms:slide_out_opponent_sprite()
  local sprite = self.opponent_sprite
  local position = sprite:get_component('Position')
  local target = { x = 1.2, y = position.world.y }
  self:slide_out_sprite(sprite, target)
end

function BattlePlatforms:enter_soul(soul, sprite, platform)
  local data = soul:get_static_data()
  local animation = sprite:get_component('Animation')
  animation:begin(data.front_sprite)
  
  local position = sprite:get_component('Position')
  local offset = position.offset
  tdengine.attach_position(sprite.id, platform.id, offset.x, offset.y)
end

function BattlePlatforms:enter_opponent_soul(soul)
  self:enter_soul(soul, self.opponent_sprite, self.opponent_platform)
end

function BattlePlatforms:enter_player_soul(soul)
  self:enter_soul(soul, self.player_sprite, self.player_platform)
end
