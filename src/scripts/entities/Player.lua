local inspect = require('inspect')
local GLFW = require('glfw')

Player = tdengine.entity('Player')
function Player:init(params)
  tdengine.register_collider(self.id)
  tdengine.register_player(self.id)
  self.speed = tdengine.vec2(.005, .005)

  self:persist()
  self:should_save(false)
  
  self.movmement_enabled = true
end

function Player:handle_movement()
  if not self.movement_enabled then return end
  
  local input = self:get_component('Input')

  if input:is_down(GLFW.Keys.I) then
	tdengine.move_entity_by_offset(self.id, 0, self.speed.y, 0)
  end
  if input:is_down(GLFW.Keys.J) then
	tdengine.move_entity_by_offset(self.id, -self.speed.x, 0, 0)
  end
  if input:is_down(GLFW.Keys.K) then
	tdengine.move_entity_by_offset(self.id, 0, -self.speed.y, 0)
  end
  if input:is_down(GLFW.Keys.L) then
	tdengine.move_entity_by_offset(self.id, self.speed.x, 0, 0)
  end
  
  local animation = self:get_component('Animation')
  if input:was_pressed(GLFW.Keys.I) then
	animation:begin('red_walkup', { loop = true })
  end
  if input:was_pressed(GLFW.Keys.K) then
	animation:begin('red_walkdown', { loop = true })
  end
  if input:was_pressed(GLFW.Keys.J) then
	animation:begin('red_walkleft', { loop = true })
  end
  if input:was_pressed(GLFW.Keys.L) then
	animation:begin('red_walkright', { loop = true })
  end

end

function Player:update(dt)
  self:handle_movement()

  local input = self:get_component('Input')
  if input:was_pressed(GLFW.Keys.O) then
	tdengine.do_interaction_check();
  end
end

-- Even though the player isn't saved to the scene, we still need to save player data.
function Player:save()
  local player = {}

  local position = self:get_component('Position')
  player.position = position.world

  return player
end

function Player:enable_movement()
  self.movement_enabled = true
end

function Player:disable_movement()
  self.movement_enabled = false
end
