local inspect = require('inspect')
local GLFW = require('glfw')

Player = tdengine.entity('Player')
function Player:init(params)
   tdengine.register_collider(self.id)
   tdengine.register_player(self.id)
end

function Player:handle_movement()
  local input = self:get_component('Input')
  local movement = self:get_component('Movement')

  if input:is_down(GLFW.Keys.I) then
	movement:move(0, .5)
  end
  if input:is_down(GLFW.Keys.J) then
	movement:move(-.5, 0)
  end
  if input:is_down(GLFW.Keys.K) then
	movement:move(0, -.5)
  end
  if input:is_down(GLFW.Keys.L) then
	movement:move(.5, 0)
  end

  local animation = self:get_component('Animation')
  if input:was_pressed(GLFW.Keys.I) then
	animation:begin('boon_walkup', { loop = true })
  end
  if input:was_pressed(GLFW.Keys.K) then
	animation:begin('boon_walkdown', { loop = true })
  end

  self.is_i_down = input:is_down(GLFW.Keys.I)
  if (not input:is_down(GLFW.Keys.I)) and (not input:is_down(GLFW.Keys.K)) then
	local editor = tdengine.find_entity('Editor')
	print('standing on: ' .. tostring(editor.frame))
	animation:begin('boon_stand', { loop = true })
  end
end

function Player:update(dt)
   self:handle_movement()

   local aabb = self:get_component('BoundingBox')
   local position = self:get_component('Position')
   aabb:draw()

   local input = self:get_component('Input')
   if input:was_pressed(GLFW.Keys.O) then
	  tdengine.do_interaction_check();
   end
end
