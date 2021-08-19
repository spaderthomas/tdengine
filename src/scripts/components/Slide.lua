Slide = tdengine.component('Slide')

function Slide:init(params)
  self.waypoints = params.waypoints
  for index, waypoint in pairs(self.waypoints) do
	self.waypoints[index] = tdengine.vec2(waypoint)
  end
  self.times = params.times
  self.target_waypoint = 0

  self.start = false
  self.done = false
end

function Slide:late_init()
  self.position = self.parent:get_component('Position')
end

function Slide:next_waypoint()
  self.target_waypoint = self.target_waypoint + 1
  local waypoint = self.waypoints[self.target_waypoint]
  local time = self.times[self.target_waypoint]
  self.dps = waypoint:subtract(self.position.world):scale(1 / time)
  self.start = true
  self.done = false
end

function Slide:slide_to_waypoint(waypoint)
  self.target_waypoint = waypoint
end

function Slide:update(dt)
  if self.start and not self.done then
	local distance = self.dps:scale(dt)
	local id = self.parent:get_id()
	tdengine.teleport_entity_by_offset(id, distance.x, distance.y)

	local target = self.waypoints[self.target_waypoint]
	self.done = target:equals(self.position.world, .0001)
  end
end
