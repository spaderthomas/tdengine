local inspect = require('inspect')
local GLFW = require('glfw')

Player = tdengine.entity('Player')
function Player:init()
   tdengine.register_collider(self:get_id())
end

function Player:handle_movement()
  local input = self:get_component('Input')
  local movement = self:get_component('Movement')

  if input:is_down(GLFW.Keys.I) then
	movement:move(0, .3)
  end
  if input:is_down(GLFW.Keys.J) then
	movement:move(-.3, 0)
  end
  if input:is_down(GLFW.Keys.K) then
	movement:move(0, -.3)
  end
  if input:is_down(GLFW.Keys.L) then
	movement:move(.3, 0)
  end
end

function Player:update(dt)
   self:handle_movement()

   local aabb = self:get_component('BoundingBox')
   local position = self:get_component('Position')
   aabb:draw()

   self:all_components()

   local graphic = self:get_component('Graphic')
end
