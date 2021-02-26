local Animation = tdengine.action('Animation')

function Animation:init(params)
   self.entity = params.entity
   self.animation = params.animation
   self.play_when_finished = params.play_when_finished
   self.wait_for_current_animation = params.wait_for_current_animation
   self.loop = ternary(params.loop, params.loop, false)
   self.began = false
end

function Animation:update(dt)
   if not self.began then
	  local entity = tdengine.find_entity(self.entity)
	  if not entity then
		 print('tdengine.actions.Animation:update() :: could not find entity: ' .. self.entity)
		 self.done = true
		 return
	  end
	  
	  local animation = entity:get_component('Animation')
	  if not animation then
		 print('tdengine.actions.Animation:update() :: entity did not have animation component: ' .. self.entity)
		 self.done = true
		 return
	  end

	  if self.wait_for_current_animation and not animation.finished_this_frame then return end
	  
	  local params = {
		loop = self.loop,
		play_when_finished = self.play_when_finished
	  }
	  animation:begin(self.animation, params)

	  self.began = true
   end
   
   -- If you're looping, no need to wait for the animation to finish
   if self.loop then
	  self.done = true
   -- But if you aren't, wait until the animation is finished
   else
	  local entity = tdengine.find_entity(self.entity)
	  local animation = entity:get_component('Animation')
	  if animation.played_requested_animation then
		self.done = true
	  end
   end
end

tdengine.actions.Animation = Animation
