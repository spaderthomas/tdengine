BoundingBox = tdengine.component('BoundingBox')
function BoundingBox:init()
   self.extents = { x = .1, y = .1 }
end

function BoundingBox:update()
end

function BoundingBox:draw(color)
   color = color or tdengine.colors.red
   local position = self.parent:get_component('Position')
   local rect = {
      origin = position.world,
	  extents = self.extents
   }
   tdengine.draw.rect_outline_world(rect, color)
end
