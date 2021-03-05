local DisappearIntoPlayer = tdengine.action('DisappearIntoPlayer')

function DisappearIntoPlayer:init(params)
  self.entity = params.entity
  local entity = tdengine.find_entity(self.entity)
  if not entity then self.done = true; return end
  
  local position = tdengine.vec2(entity:get_component('Position').world)
  
  self.target = tdengine.vec2(tdengine.find_entity('Player'):get_component('Position').world)
  local initial_difference = self.target:subtract(position)
  self.per_frame = initial_difference:scale(.025) -- do it in 40 frames
end

function DisappearIntoPlayer:update(dt)
  local entity = tdengine.find_entity(self.entity)
  if not entity then self.done = true; return end
   
  local position = tdengine.vec2(entity:get_component('Position').world)
  if self.target:equals(position, .001) then
	tdengine.destroy_entity(entity.id)
	self.done = true
	return
  end

  position = position:add(self.per_frame)
  local flags = tdengine.flags.physics.bypass
  tdengine.move_entity_absolute(entity.id, position.x, position.y, flags)
end

tdengine.actions.DisappearIntoPlayer = DisappearIntoPlayer
