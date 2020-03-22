local inspect = require('inspect')
local GLFW = require('glfw')

local animations = {
  spader_stand = {
	'spader_stand_1.png'
  },
  spader_walk_down = {
	'spader_walk_down1.png',
	'spader_walk_down2.png',
	'spader_walk_down3.png',
	'spader_walk_down4.png',
	'spader_walk_down5.png',
	'spader_walk_down6.png'
  }
}

Spader = tdengine.entity('Spader')
function Spader:init()
  local graphic = self:add_component('Graphic')
  graphic.scale = { x = .1, y = .1 }
  
  local animation = self:add_component('Animation')
  animation:batch_add(animations)
  animation:begin('spader_walk_down')
  
  local position = self:add_component('Position')
  local movement = self:add_component('Movement')
  local input = self:add_component('Input')
end

function Spader:handle_movement()
  local position = self:get_component('Position')
  local movement = self:get_component('Movement')

  local input = self:get_component('Movement')
  if input:was_key_pressed(GLFW.Keys.I) then
	self.movement:move(0, 1)
  end
  if input:was_key_pressed(GLFW.Keys.J) then
	self.movement:move(-1, 0)
  end
  if input:was_key_pressed(GLFW.Keys.K) then
	self.movement:move(0, -1)
  end
  if input:was_key_pressed(GLFW.Keys.L) then
	self.movement:move(1, 0)
  end
  
end

function Spader:update(dt)
  --self:handle_movement()
end


