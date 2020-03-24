local inspect = require('inspect')
local GLFW = require('glfw')

local animations = {
  wilson_walk_down = {
	'wilson_walk_down1.png',
	'wilson_walk_down2.png',
	'wilson_walk_down3.png',
	'wilson_walk_down4.png',
	'wilson_walk_down5.png',
	'wilson_walk_down6.png'
  }
}

Wilson = tdengine.entity('Wilson')
function Wilson:init()
  local graphic = self:add_component('Graphic')
  graphic.scale = { x = .1, y = .1 }
  
  local animation = self:add_component('Animation')
  animation:batch_add(animations)
  animation:begin('wilson_walk_down')

  local physics = self:add_component('Physics')
  local movement = self:add_component('Movement')
  local input = self:add_component('Input')
end

function Wilson:handle_movement()
  local position = self:get_component('Position')
  local movement = self:get_component('Movement')

  local input = self:get_component('Input')
  if input:was_key_pressed(GLFW.Keys.FIVE) then
	movement:move(0, 1)
  end
  if input:was_key_pressed(GLFW.Keys.ONE) then
	movement:move(-1, 0)
  end
  if input:was_key_pressed(GLFW.Keys.TWO) then
	movement:move(0, -1)
  end
  if input:was_key_pressed(GLFW.Keys.THREE) then
	movement:move(1, 0)
  end
  
end

function Wilson:update(dt)
   self:handle_movement()
   local c = self:get_component('Movement')
end


