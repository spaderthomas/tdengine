Battle = tdengine.entity('Battle')

local BattleState = {
  MovingPlatforms = 'MovingPlatforms',
  Sup = 'Sup'
}

-- Trainer platforms
local PlatformType = {
  Player = 'player_platform',
  Opponent = 'opponent_platform'
}

local platforms = {}
platforms[PlatformType.Player] = {
  name = 'Platform',
  tag = PlatformType.Player,
  components = {
	Position = {
	  world = { x = -.25, y = .3 }
	},
	Animation = {
	  current = 'platform_grass_player'
	}
  }
}
platforms[PlatformType.Opponent] = {
  name = 'Platform',
  tag = PlatformType.Opponent,
  components = {
	Position = {
	  world = { x = 1.3, y = .6 }
	},
	Animation = {
	  current = 'platform_grass_opponent'
	}
  }
}

local platform_positions = {}
platform_positions[PlatformType.Player] = .3
platform_positions[PlatformType.Opponent] = .65

-- Background
local background = {
  components = {
	Animation = {},
	Position = {
	  world = {
		x = 3,
		y = 3
	  }
	}
  },
  name = "Background",
  params = {}
}

-- Trainer sprites
local opponent_sprite =   {
  name = 'StaticNpc',
  tag = 'opponent',
  components = {
	Animation = {
	  layer = 2
	},
	Position = {
	  tag = PlatformType.Opponent
	}
  }
}


function Battle:init(params)
  self.state = BattleState.MovingPlatforms
  self.platform_time = 1
  self.platform_info = {}
  tdengine.move_camera(0, 0)

  self.data = tdengine.fetch_module_data('battle/battles/' .. params.battle)
  
  background.components.Animation.current = self.data.background
  tdengine.create_entity('Background', background)

end

function Battle:setup()
  self:load_platform(PlatformType.Player)
  self:load_platform(PlatformType.Opponent)
  
  opponent_sprite.components.Animation.current = self.data.trainer
  tdengine.create_entity('StaticNpc', opponent_sprite)
end

function Battle:load_platform(which)
  tdengine.create_entity('Platform', platforms[which])
  local platform = tdengine.find_entity_by_tag(which)
  local position = platform:get_component('Position')
  local info = {
	done = false,
	distance = platform_positions[which] - position.world.x
  }

  self.platform_info[which] = info
end

function Battle:cleanup()
end

function Battle:advance_platform(which, dt)
  local platform = tdengine.find_entity_by_tag(which)
  local position = platform:get_component('Position')
  if double_eq(position.world.x, platform_positions[which], .01) then
	self.platform_info[which].done = true
	return true
  end
  
  local dps = self.platform_info[which].distance / self.platform_time
  local distance = dps * dt
  local id = platform:get_id()

  tdengine.teleport_entity(id, position.world.x + distance, position.world.y)
  
end

function Battle:update(dt)
  if self.state == BattleState.MovingPlatforms then
	local player_done = self:advance_platform(PlatformType.Player, dt)
	local opponent_done = self:advance_platform(PlatformType.Opponent, dt)

	if player_done and opponent_done then
	  self.state = BattleState.Sup
	end
  end
end
