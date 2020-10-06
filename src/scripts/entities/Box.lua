Box = tdengine.entity('Box')
function Box:init()
   self:add_component('Physics')
end

function Box:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw(tdengine.colors.red)
end
