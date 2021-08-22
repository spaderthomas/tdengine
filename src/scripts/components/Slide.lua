Slide = tdengine.component('Slide')

function Slide:init(params)
  self.waypoints = params.waypoints
  self:convert_waypoints_to_vector()
  self.times = params.times
  self.loop = params.loop or false

  
  self.target_waypoint = 0

  self.start = false
  self.done = true
end

function Slide:late_init()
  self.position = self.parent:get_component('Position')
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

function Slide:next_waypoint()
  self.target_waypoint = self.target_waypoint + 1
  if self.target_waypoint > #self.waypoints then
	if self.loop then
	  self.target_waypoint = 0
	else
	  self.target_waypoint = #self.waypoints
	end
  end
  
  local waypoint = self.waypoints[self.target_waypoint]
  local time = self.times[self.target_waypoint]
  self.dps = waypoint:subtract(self.position.world):scale(1 / time)
  self.start = true
  self.done = false
end

function Slide:set_waypoints(waypoints, times)
  self.waypoints = waypoints
  self:convert_waypoints_to_vector()
  self.times = times
  self.target_waypoint = 0
end

function Slide:convert_waypoints_to_vector()
  for index, waypoint in pairs(self.waypoints) do
	self.waypoints[index] = tdengine.vec2(waypoint)
  end
end
