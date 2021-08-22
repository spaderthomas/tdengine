Position = tdengine.component('Position')
function Position:init(params)
  self.world = {}
  
  self.params = params
  if params.world then
	-- Shallow copy!
	self.world.x = params.world.x
	self.world.y = params.world.y
  elseif params.marker then
	local marker = tdengine.markers[params.marker]
	if marker then
	  self.world = { x = marker.x, y = marker.y }
	end
  elseif params.tag then
	local entity = tdengine.find_entity_by_tag(params.tag)
	local offset = params.offset or tdengine.vec2(0, 0)
	if entity then
	  tdengine.attach_position(self.parent.id, entity.id, offset.x, offset.y)
	  self.world = { x = 0, y = 0 }
	end
	self.offset = offset
  end

  -- Did whatever method we picked fail to actually fill in the position?
  if not self.world.x or not self.world.y then
	local entity_string = self.parent:get_name() .. '(' .. inspect(self.parent.id) .. ')'
	tdengine.log('@bad_position_params: ' .. entity_string)
	tdengine.log('here are the bad position params: ' .. inspect(params))
	self.world = { x = 0, y = 0 }
  end

  self.original_position = self.world
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

function Position:return_to_original()
  tdengine.teleport_entity(self.parent.id, self.original_position.x, self.original_position.y)
end

function Position:update()
end
