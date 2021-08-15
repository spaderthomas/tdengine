BattleHudBall = tdengine.entity('BattleHudBall')

BattleHudBall.Kind = {
  Player = 'Player',
  Opponent = 'Opponent',
}
BattleHudBall.BaseFinalPositions = {}
BattleHudBall.BaseFinalPositions[BattleHudBall.Kind.Player] = .24
BattleHudBall.BaseFinalPositions[BattleHudBall.Kind.Opponent] = .24

local ball_spacing = .03

function BattleHudBall:init(params)
  self.index = params.index
  self.time = params.time

  --local base_final_position = 
  self.final_position = .24 - (self.index * ball_spacing)

  self.position = self:get_component('Position')
  self.dps = (self.final_position - self.position.world.x) / self.time

  self.start = false
  self.done = false

  self.battle = tdengine.find_entity('Battle')

  local animation = self:get_component('Animation')
  animation:begin('battle_hud_ball')
end

function BattleHudBall:update(dt)
  --local
  if self.start and not self.done then
	local distance = self.dps * dt
	local id = self:get_id()
	tdengine.teleport_entity_by_offset(id, distance, 0)
	
	self.done = double_eq(self.position.world.x, self.final_position, .0001)
  end
end

