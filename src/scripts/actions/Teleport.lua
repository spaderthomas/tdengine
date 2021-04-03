local Teleport = tdengine.action('Teleport')

function Teleport:init(params)
   self.descriptor = {
	  entity = params.entity,
	  tag = params.tag,
	  id = params.id
   }

   if params.marker then
	 self.dest = tdengine.markers[params.marker]
   elseif params.teleport_near_player then
	  local player = tdengine.find_entity('Player')
	  local position = player:get_component('Position').world
	  self.dest = {
		 x = position.x + params.offset.x,
		 y = position.y + params.offset.y
	  }
   end
end

function Teleport:update(dt)
   local entity = tdengine.find_entity_by_any(self.descriptor)
   if entity then tdengine.teleport_entity(entity.id, self.dest.x, self.dest.y) end
   self.done = true
end

tdengine.actions.Teleport = Teleport
