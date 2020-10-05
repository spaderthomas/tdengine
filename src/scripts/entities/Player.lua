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

Player = tdengine.entity('Player')
function Player:init()
  local graphic = self:add_component('Graphic')
  graphic.scale = { x = .1, y = .1 }
  
  local animation = self:add_component('Animation')
  animation:batch_add(animations)
  animation:begin('spader_walk_down')
  
  local physics = self:add_component('Physics')
  local movement = self:add_component('Movement')
  local input = self:add_component('Input')
end

function Player:handle_movement()
  local input = self:get_component('Input')
  local movement = self:get_component('Movement')

  if input:is_key_down(GLFW.Keys.I) then
	movement:move(0, .3)
  end
  if input:is_key_down(GLFW.Keys.J) then
	movement:move(-.3, 0)
  end
  if input:is_key_down(GLFW.Keys.K) then
	movement:move(0, -.3)
  end
  if input:is_key_down(GLFW.Keys.L) then
	movement:move(.3, 0)
  end
  
end

function Player:update(dt)
   self:handle_movement()

   local aabb = self:get_component('BoundingBox')
   local position = self:get_component('Position')
   aabb:draw()
end


