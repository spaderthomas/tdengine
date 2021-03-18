Background = tdengine.entity('Background')
function Background:init(params)
   tdengine.teleport_entity(self.id, 3, 3)
end

function Background:pre_save()
   local animation = self:get_component('Animation')
   animation:should_save(true)
end

function Background:update(dt)
end
