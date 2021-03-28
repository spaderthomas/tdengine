local Walk = tdengine.action('Walk')

function Walk:init(params)
  self.descriptor = {
	 entity = params.entity,
	 tag = params.tag,
	 id = params.id
  }

  self.markers = params.markers
  self.current = 0
  self.speed = ternary(params.speed, params.speed, .025)

  self:set_up_next_waypoint()
end

function Walk:set_up_next_waypoint()
  -- No more markers? We're finished
  if #self.markers == self.current then
	 self.done = true
	 return
  end

  self.current = self.current + 1
  
  local entity = tdengine.find_entity_by_any(self.descriptor)
  if not entity then self.done = true; return end

  local current = tdengine.vec2(entity:get_component('Position').world)
  local marker = tdengine.markers[self.markers[self.current]]
  local target = tdengine.vec2(marker.x, marker.y)
  
  local initial_difference = target:subtract(current)
  self.per_frame = initial_difference:scale(self.speed)
  self.target = target
end

function Walk:update(dt)
  local entity = tdengine.find_entity_by_any(self.descriptor)
  local position = tdengine.vec2(entity:get_component('Position').world)
  if self.target:equals(position, .001) then
	self:set_up_next_waypoint()
	return
  else
	 tdengine.move_entity_by_offset(entity.id, self.per_frame.x, self.per_frame.y, 0)
  end
end

tdengine.actions.Walk = Walk
