local DestroyEntity = tdengine.action('DestroyEntity')

function DestroyEntity:init(params)
   self.descriptor = {
	  entity = params.entity,
	  tag = params.tag,
	  id = params.id
   }
end

function DestroyEntity:update(dt)
   local entity = tdengine.find_entity_by_any(self.descriptor)
   self.done = true
end

tdengine.actions.DestroyEntity = DestroyEntity
