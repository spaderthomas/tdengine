Box = tdengine.entity('Box')
function Box:init()
   tdengine.internal.register_collider(self:get_id())
end

function Box:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw(tdengine.colors.red)
end
