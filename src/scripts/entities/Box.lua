Box = tdengine.entity('Tom')
function Box:init()
   local position = self:add_component('Position')
   local physics = self:add_component('Physics')
end

function Box:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw(tdengine.colors.red)
end
