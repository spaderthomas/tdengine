BoundingBox = tdengine.component('BoundingBox')
function BoundingBox:init(params)
   self.extents = params.extents or { x = .1, y = .1 }
   self.offset = params.offset or { x = 0, y = 0 }
end

function BoundingBox:save()
   return {
	  extents = self.extents,
	  offset = self.offset,
   }
end

function BoundingBox:update()
end

function BoundingBox:draw(color)
   color = color or tdengine.colors.red

   local graphic = self.parent:get_component('Graphic')
   if graphic then
	  local flag = tdengine.render_flags.highlighted
	  local hl = bitwise(tdengine.op_and, graphic.flags, flag) ~= 0
	  color = ternary(hl, tdengine.colors.blue, color)
   end
   
   local position = self.parent:get_component('Position')
   local rect = {
      origin = tdengine.vec2(position.world):add(self.offset),
	  extents = self.extents
   }
   tdengine.draw.rect_outline_world(rect, color)
end
