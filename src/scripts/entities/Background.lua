Background = tdengine.entity('Background')
function Background:init(params)
   -- Engine needs to know the position of the thing to render it
   tdengine.register_collider(self:get_id())
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
