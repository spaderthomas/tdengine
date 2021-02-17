local inspect = require('inspect')

Position = tdengine.component('Position')
function Position:init(params)
   self.world = params.world or { x = 0, y = 0 }
end

function Position:late_init()
   local name = self.parent:get_name()
   tdengine.register_collider(self.parent.id)
   tdengine.teleport_entity(self.parent:get_id(), self.world.x, self.world.y)
end

function Position:save()
   return {
	  world = { x = self.world.x, y = self.world.y }
   }
end

function Position:update()
end
