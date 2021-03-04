local DisappearIntoPlayer = tdengine.action('DisappearIntoPlayer')

function DisappearIntoPlayer:init(params)
  self.entity = params.entity
  local entity = tdengine.find_entity(self.entity)
  local position = tdengine.vec2(entity:get_component('Position').world)
  
  self.target = tdengine.vec2(tdengine.find_entity('Player'):get_component('Position').world)
  local initial_difference = self.target:subtract(position)
  self.per_frame = initial_difference:scale(.05) -- do it in 20 frames
  print('initted')
end

function DisappearIntoPlayer:update(dt)
  local entity = tdengine.find_entity(self.entity)
  local position = tdengine.vec2(entity:get_component('Position').world)
  if self.target:equals(position) then
	self.done = true
	return
  end
						 
  local next_position = position:add(self.per_frame)
  self.next = next_position
  tdengine.move_entity_absolute(entity.id, next_position.x, next_position.y)
end

tdengine.actions.DisappearIntoPlayer = DisappearIntoPlayer
