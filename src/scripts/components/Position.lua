Position = tdengine.component('Position')
function Position:init(params)
   self.world = params.world or { x = 0, y = 0 }
   tdengine.register_position(self.parent.id)
end

function Position:late_init()
   tdengine.teleport_entity(self.parent.id, self.world.x, self.world.y)
end

function Position:save()
   return {
	  world = { x = self.world.x, y = self.world.y }
   }
end

function Position:update()
end
