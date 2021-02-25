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
   self.finished_one_loop = false

   -- The engine will read from this when you ask it to draw
   self.sprite = ''

   -- All the animation data
   self.animations = params.animations or {}

   self.last_animation = params.current
   print(params.current)
   self:begin(params.current)
end

-- Some entities need to save out their animation table, because they are built
-- at runtime. This is the equivalent of Animation:save, except we can't call it
-- that, because that would automatically opt-in the component to serialization
function Animation:save()
   return {
	  animations = self.animations,
	  current = self.current
   }
end

function Animation:update(dt)
  local data = self.animations[self.current]
  self.time_to_next = self.time_to_next - dt

  -- Move onto the next frame
  if self.time_to_next <= 0 then
	self.frame = self.frame + 1
	self.finished_one_loop = true

	-- If we're finished, move onto the next animation
	if (self.frame + 1) > self.current_animation.count then
	  if self.loop then
		-- @hack: If we kicked this thing off with any other params, they will get lost when
		-- we start looping. Not a problem now.
		local params = { loop = true }
		self:begin(self.current_animation.name, params)
	  else 
		local params = { loop = true }
		self:begin(self.last_animation, params)
	  end
	end

	-- Otherwise, advance the frame
	local next_frame = self.current_animation.data[frame]
	if true then return end
	self.sprite = next_frame.name
	self.time_to_next = next_frame.time
  end
end

function Animation:begin(name, params)
  params = params or {}
  self.loop = params.loop or false
  self.finished_one_loop = false
  
  self.frame = 1
  self.current_animation = {
	name = name,
	data = self.animations[name],
	count = table.getn(self.animations[name])	
  }
  
  self.time_to_next = self.current_animation.data[1].time
  self.sprite = self.current_animation.data[1].sprite
end
