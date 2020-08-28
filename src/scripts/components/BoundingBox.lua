BoundingBox = tdengine.component('BoundingBox')
function BoundingBox:init()
   self.extents = { x = .1, y = .1 }
end

function BoundingBox:update()
end

function BoundingBox:draw(color)
   color = color or tdengine.colors.red
   local position = self.parent:get_component('Position')
   tdengine.draw.outline_rect_world(position.world.x, position.world.y, self.extents.x, self.extents.y, color.r, color.g, color.b, color.a)
end
