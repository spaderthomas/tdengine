SlidingSprite = tdengine.entity('SlidingSprite')
function SlidingSprite:init(params)
  self.final_position = params.final_position
  self.time = params.time

  self.position = self:get_component('Position')
  self.dps = self.final_position:subtract(self.position.world):scale(1 / self.time)

  self.start = false
  self.done = false
end

function SlidingSprite:update(dt)
  if self.start and not self.done then
	local distance = self.dps:scale(dt)
	local id = self:get_id()
	tdengine.teleport_entity_by_offset(id, distance.x, distance.y)

	self.done = self.final_position:equals(self.position.world, .0001)
  end

end
