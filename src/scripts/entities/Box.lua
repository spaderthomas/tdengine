Box = tdengine.entity('Box')
function Box:init()
end

function Box:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw(tdengine.colors.red)
end
