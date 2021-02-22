BoundingBox = tdengine.component('BoundingBox')
function BoundingBox:init(params)
   self.extents = params.extents or { x = .1, y = .1 }
   self.offset = params.offset or { x = 0, y = 0 }
   self:should_save(false)
end

-- Box wants to save its bounding box, because it doesn't really
-- have a prefab.
function BoundingBox:save()
   return {
	  extents = self.extents,
	  offset = self.offset,
   }
end

function BoundingBox:update()
end

function BoundingBox:draw(color)
   -- Figure out the color based on whether it's highlighted in the Editor
   color = color or tdengine.colors.red

   local editor = tdengine.find_entity('Editor')
   if editor and editor.selected then
	  if editor.selected.id == self.parent.id then
		 color = tdengine.colors.blue
	  end
   end
   
   local position = self.parent:get_component('Position')
   local rect = {
      origin = tdengine.vec2(position.world):add(self.offset),
	  extents = self.extents
   }
   tdengine.draw.rect_outline_world(rect, color)
end
