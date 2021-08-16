SlidingSprite = tdengine.entity('SlidingSprite')
function SlidingSprite:init(params)
  self.waypoints = params.waypoints
  for index, waypoint in pairs(self.waypoints) do
	self.waypoints[index] = tdengine.vec2(waypoint)
  end
  self.target_waypoint = 0
  self.times = params.times

  self.position = self:get_component('Position')

  self.start = false
  self.done = false
end

function SlidingSprite:next_waypoint()
  self.target_waypoint = self.target_waypoint + 1
  local waypoint = self.waypoints[self.target_waypoint]
  local time = self.times[self.target_waypoint]
  self.dps = waypoint:subtract(self.position.world):scale(1 / time)
  self.start = true
  self.done = false
end

function SlidingSprite:slide_to_waypoint(waypoint)
  self.target_waypoint = waypoint
end

function SlidingSprite:update(dt)
  if self.start and not self.done then
	local distance = self.dps:scale(dt)
	local id = self:get_id()
	tdengine.teleport_entity_by_offset(id, distance.x, distance.y)

	local target = self.waypoints[self.target_waypoint]
	self.done = target:equals(self.position.world, .0001)
  end

end
