Vision = tdengine.component('Vision')
function Vision:init(params)
   self.extents = params.extents or { x = .1, y = .1 }
   self.offset = params.offset or { x = 0, y = 0 }

   self.world = params.world or { x = 0, y = 0 }
   tdengine.register_position(self.parent.id)
end

function Position:late_init()
   tdengine.teleport_entity(self.parent.id, self.world.x, self.world.y)
end

function Vision:save()
   return {
	  extents = self.extents,
	  offset = self.offset,
   }
end 

function Vision:update()
   local position = self.parent:get_component('Position')
   local rect = {
      origin = tdengine.vec2(position.world):add(self.offset),
	  extents = self.extents
   }
   tdengine.draw.rect_outline_world(rect, tdengine.colors.green)
end
