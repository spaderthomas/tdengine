Tom = tdengine.entity('Tom')
function Tom:init()
   local position = self:add_component('Position')
   print(position)
   local physics = self:add_component('Physics')
   local movement = self:add_component('Movement')
   print('Created Tom with EntityID: ' .. self:get_id())
end

function Tom:update(dt)
   local position = self:get_component('Position')
   position.world = { x = .5, y = .5 }
   local aabb = self:get_component('BoundingBox')
   aabb:draw()
end
