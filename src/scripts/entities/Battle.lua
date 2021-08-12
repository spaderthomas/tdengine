Battle = tdengine.entity('Battle')

local PlatformType = {
  Player = 'player_platform',
  Opponent = 'opponent_platform'
}
local BattleState = {
  MovingPlatforms = 'MovingPlatforms'
}

local platform_positions = {}
platform_positions[PlatformType.Player] = .3
platform_positions[PlatformType.Opponent] = .65

local final_player_platform_x = .42
local final_opponent_platform_x = .65

function Battle:init(params)
  self.state = BattleState.MovingPlatforms
  self.platform_time = 1
  self.platform_info = {}
  tdengine.move_camera(0, 0)
end

function Battle:setup()
  self:load_platform(PlatformType.Player)
  self:load_platform(PlatformType.Opponent)
end

function Battle:load_platform(which)
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
	return
  end
  
  local dps = self.platform_info[which].distance / self.platform_time
  local distance = dps * dt
  local id = platform:get_id()
  tdengine.teleport_entity(id, position.world.x + distance, position.world.y)
  
end

function Battle:update(dt)
  if self.state == BattleState.MovingPlatforms then
	self:advance_platform(PlatformType.Player, dt)
	self:advance_platform(PlatformType.Opponent, dt)
  end
end
