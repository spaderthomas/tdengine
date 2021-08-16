BattleHudBall = tdengine.entity('BattleHudBall')

BattleHudBall.State = {
  Hidden = 'Hidden',
  Sliding = 'Sliding',
  FinishedSliding = 'FinishedSliding'
}

BattleHudBall.Kind = {
  Player = 'Player',
  Opponent = 'Opponent',
}
BattleHudBall.BaseRestingPositions = {}
BattleHudBall.BaseRestingPositions[BattleHudBall.Kind.Player] = .24
BattleHudBall.BaseRestingPositions[BattleHudBall.Kind.Opponent] = .24
BattleHudBall.HiddenPositions = {}
BattleHudBall.HiddenPositions[BattleHudBall.Kind.Player] = -.02
BattleHudBall.HiddenPositions[BattleHudBall.Kind.Opponent] = 1.02

local ball_spacing = .03

function BattleHudBall:init(params)
  self.index = params.index
  self.time_in = params.time_in
  self.time_out = params.time_out
  self.team = params.team
  self.kind = params.kind
  
  self.state = BattleHudBall.State.Hidden
  self.final_position = 0
  self.dps = 0
  
  self.battle = tdengine.find_entity('Battle')
  self.position = self:get_component('Position')
  local animation = self:get_component('Animation')
  animation:begin('battle_hud_ball')
end

function BattleHudBall:update(dt)
  if self.state == BattleHudBall.State.Sliding then
	local distance = self.dps * dt
	local id = self:get_id()
	tdengine.teleport_entity_by_offset(id, distance, 0)
	
	if double_eq(self.position.world.x, self.final_position, .0001) then
	  self.state = BattleHudBall.State.FinishedSliding
	end
  end
  --self:update_sprite()
end

function BattleHudBall:slide_in()
  local base_position = BattleHudBall.BaseRestingPositions[self.kind]
  local offset = self.index * ball_spacing
  self.final_position = base_position - offset

  self.dps = (self.final_position - self.position.world.x) / self.time_in

  self.state = BattleHudBall.State.Sliding
end

function BattleHudBall:slide_out()
  self.final_position = BattleHudBall.HiddenPositions[self.kind]

  self.dps = (self.final_position - self.position.world.x) / self.time_out
  
  self.state = BattleHudBall.State.Sliding
end

function BattleHudBall:update_sprite()
  local soul = self.team[self.index]
  print(inspect(soul))
end
