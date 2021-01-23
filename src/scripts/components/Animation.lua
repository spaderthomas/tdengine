local DEFAULT_FRAME_TIME = 8 / 60

Animation = tdengine.component('Animation')
function Animation:load(data)
   self.animations = data.animations or {}
   self:batch_add(self.animations)

   self.loop = true
   self.done = false

   self.current = ''
   self.last = ''
   local current = data.current or nil
   if current then
	  self.current = current
	  self.last = current
	  self:begin(self.current)
   end
end

-- Some entities need to save out their animation table, because they are built
-- at runtime. This is the equivalent of Animation:save, except we can't call it
-- that, because that would automatically opt-in the component to serialization
function Animation:as_table()
   return {
	  animations = self.animations,
	  current = self.current
   }
end

function Animation:init()
   self.frame = 0
   self.time_to_next = DEFAULT_FRAME_TIME
end

function Animation:update(dt)
   self.time_to_next = self.time_to_next - dt
   if self.time_to_next <= 0 then
	  local frames = tdengine.get_frames(self.current)
	  if self.loop then
		 if (self.frame + 1) >= #frames then
			-- Not sure if this is totally correct...? This is here because I need
			-- a way to signal to the action that triggers it that it can unblock.
			self.done = true
		 end
		 self.frame = (self.frame + 1) % #frames
	  else
		 -- We finished; reset to the last animation and loop it
		 self.frame = self.frame + 1
		 if self.frame >= #frames then
			self.frame = 0
			self.current = self.last

			-- Maybe it makes more sense to hang on the last frame rather than
			-- remember the last animation?
			self.loop = true
			self.done = true
		 end
	  end
	  self.time_to_next = DEFAULT_FRAME_TIME
   end
end

function Animation:add(name, frames)
   tdengine.register_animation(name, frames)
end

function Animation:batch_add(animations)
   for name, frames in pairs(animations) do
	  tdengine.register_animation(name, frames)
   end
end

function Animation:begin(name, params)
   params = params or {}
   self.loop = ternary(params.loop, true, false)
   self.done = false
   self.current = name
   self.frame = 0
end
