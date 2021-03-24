local direction = {
  left = 0,
  right = 1
}

Eddy = tdengine.entity('Eddy')
function Eddy:init(params)
  tdengine.register_collider(self.id)

  local animation = self:get_component('Animation')
  animation:begin('eddy_walkright')
  self.direction = direction.right
end

function Eddy:update(dt)
  local story_marker = tdengine.state['main:story_marker']
  if story_marker == tdengine.story_markers.LEFT_JANITOR_CLOSET then
	local animation = self:get_component('Animation')
	if animation.finished_this_frame then
	  if self.direction == direction.right then
		self.direction = direction.left
		animation:begin('eddy_walkleft')
	  elseif self.direction == direction.left then
		self.direction = direction.right
		animation:begin('eddy_walkright')
	  end
	end

	local offset = ternary(self.direction == direction.right, .005, -.005)
	tdengine.move_entity_by_offset(self.id, offset, 0, 0)
  end
end
