local animations = {
  stand = {
	'tree.png'
  }
}

Tom = tdengine.entity('Tom')
function Tom:init()
   local position = self:add_component('Position')
   local physics = self:add_component('Physics')
   local movement = self:add_component('Movement')

   local graphic = self:add_component('Graphic')

   local animation = self:add_component('Animation')
   animation:batch_add(animations)
   animation:begin('stand')
end

function Tom:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw(tdengine.colors.green)
end
