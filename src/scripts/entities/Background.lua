Background = tdengine.entity('Background')
function Background:init(params)
   tdengine.teleport_entity(self:get_id(), 3, 3)
end

function Background:save()
   local animation = self:get_component('Animation')
   return {
	  components = {
		 Animation = animation:as_table()
	  }
   }
end

function Background:update(dt)
end
