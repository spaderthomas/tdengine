BattleHudBall = tdengine.entity('BattleHudBall')

local ball_spacing = .03

function BattleHudBall:init(params)
  self.index = params.index
  self.time = params.time
  self.final_position = .24 - (self.index * ball_spacing)

  self.position = self:get_component('Position')
  self.dps = (self.final_position - self.position.world.x) / self.time

  self.start_sliding = false
  self.done_sliding = false

  self.battle = tdengine.find_entity('Battle')

  local animation = self:get_component('Animation')
  animation:begin('battle_hud_ball')
end

function BattleHudBall:update(dt)
  --local
  if self.start_sliding and not self.done_sliding then
	local distance = self.dps * dt
	local id = self:get_id()
	tdengine.teleport_entity_by_offset(id, distance, 0)
	
	self.done_sliding = double_eq(self.position.world.x, self.final_position, .0001)
  end
end

