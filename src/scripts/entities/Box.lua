Box = tdengine.entity('Box')
function Box:init()
   self:add_component('Physics')
end

function Box:update(dt)
   local aabb = self:get_component('BoundingBox')
   local position = self:get_component('Position')
   local inspect = require('inspect')
   print(inspect(position.world))
   aabb:draw(tdengine.colors.red)
end
