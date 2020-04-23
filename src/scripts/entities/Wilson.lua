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

  --local physics = self:add_component('Physics')
  
  local position = self:add_component('Position')
  position.world.x = .4
  position.world.y = .4
end

function Wilson:update(dt)
end


