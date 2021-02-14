Box = tdengine.entity('Box')
function Box:init()
   tdengine.register_collider(self:get_id())
   local graphic = self:get_component('Graphic')
   graphic.draw = false
end

function Box:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw()
end
