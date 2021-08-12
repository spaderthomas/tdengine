local DEFAULT_FRAME_TIME = 8 / 60

Animation = tdengine.component('Animation')
function Animation:init(params)
   -- By default, animations live in the prefab of the entity. Not being changed at runtime.
   -- Only need to save them when we can't define in the prefab (like Background)
   self:should_save(false)

   -- Stuff to calculate what we're doing on update
   self.frame = 1
   self.time_to_next = 0
   self.loop = true
   self.error = false

   -- The engine will read from this when you ask it to draw
   self.sprite = ''

   params.current = params.current or 'unknown'
   self.last_animation = params.current
   self:begin(params.current, params)
end

-- Some entities need to save out their animation table, because they are built
-- at runtime. This is the equivalent of Animation:save, except we can't call it
-- that, because that would automatically opt-in the component to serialization
function Animation:save()
   return {
	  current = self.current_animation.name
   }
end

function Animation:update(dt)
  if self.error then return end
  self.time_to_next = self.time_to_next - dt
  self.finished_this_frame = false

  -- Move onto the next frame
  if self.time_to_next <= 0 then
	self.frame = self.frame + 1

	-- If we're finished, move onto the next animation
	if self.frame > self.current_animation.count then
	  if self.loop then
		-- @hack: If we kicked this thing off with any other params, they will get lost when
		-- we start looping. Not a problem now.
		self.frame = 1
		self.time_to_next = self.current_animation.data[1].time
		self.sprite = self.current_animation.data[1].sprite

		--local params = { loop = true }
		--self:begin(self.current_animation.name, params)
	  else
		-- You can ask to loop an animation after your request is done
		if self.play_when_finished then
		  self:begin(self.play_when_finished, { loop = true })
		-- But if you don't, we're going to stick to the last frame of this animation
		else
		  self:loop_last_frame()
		  return
		end
	  end

	  self.played_requested_animation = true
	  self.finished_this_frame = true
	end
	
	-- Otherwise, advance the frame
	local next_frame = self.current_animation.data[self.frame]
	self.sprite = next_frame.sprite
	self.time_to_next = next_frame.time
  end
end

function Animation:loop_last_frame()
  local last_frame = self.current_animation.data[self.current_animation.count]
  self.current_animation.data = {
	last_frame
  }
  self.current_animation.count = 1
  
  self.frame = 1
  self.loop = true
  self.time_to_next = self.current_animation.data[1].time
  self.sprite = self.current_animation.data[1].sprite
end

function Animation:begin(name, params)
  if not tdengine.animations[name] then
	 print('@unknown_animation: ' .. name .. ', ' .. tostring(self.parent.id))
	 self.error = true
	 self:begin('unknown', { loop = true })
	 return
  end
  
  params = params or {}
  self.loop = params.loop or false
  self.play_when_finished = params.play_when_finished or nil
  self.played_requested_animation = false
  self.finished_this_frame = false

  self.frame = 1
  self.current_animation = {
	name = name,
	data = tdengine.animations[name],
	count = table.getn(tdengine.animations[name])	
  }
  
  self.time_to_next = self.current_animation.data[1].time
  self.sprite = self.current_animation.data[1].sprite
end
