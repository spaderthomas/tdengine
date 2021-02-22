Box = tdengine.entity('Box')
function Box:init(params)
   tdengine.register_collider(self.id)

   local graphic = self:get_component('Graphic')
   graphic.draw = false
end

function Box:pre_save()
   local box = self:get_component('BoundingBox')
   box:should_save(true)
end

function Box:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw()
end
