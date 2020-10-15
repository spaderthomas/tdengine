BoundingBox = tdengine.component('BoundingBox')
function BoundingBox:load(data)
   self.extents = data.extents or { x = .1, y = .1 }
   print(self.parent:get_name() .. '(' .. self.parent:get_id() .. ')' .. ' data:')
   print(inspect(data.extents))
end

function BoundingBox:save()
   return {
	  extents = self.extents
   }
end

function BoundingBox:init()
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
