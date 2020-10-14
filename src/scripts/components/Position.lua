local inspect = require('inspect')

Position = tdengine.component('Position')
function Position:load(data)
   self.world = data.world or { x = 0, y = 0 }
end

function Position:save()
   return {
	  world = { x = self.world.x, y = self.world.y }
   }
end

function Position:init()
   tdengine.teleport_entity(self.parent:get_id(), self.world.x, self.world.y)
end

function Position:update()
end
