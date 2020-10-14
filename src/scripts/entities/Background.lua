Background = tdengine.entity('Background')
function Background:init()
   -- Engine needs to know the position of the thing to render it
   tdengine.register_collider(self:get_id())
   tdengine.teleport_entity(self:get_id(), 3, 3)
end

function Background:update(dt)
end
