local class = require('middleclass')
local inspect = require('inspect')

local DEFAULT_FRAME_TIME = 8 / 60

Animation = class('Animation')
function Animation:init()
   self.animations = {}
   self.current = ""
   self.frame = 0
   self.time_to_next = DEFAULT_FRAME_TIME

   print("Animation:init()");
end

function Animation:update(dt)
   self.time_to_next = self.time_to_next - dt
   if self.time_to_next <= 0 then
	  self:advance_frame()
	  self.time_to_next = 8 / 60
   end
end

function Animation:add(name, frames)
   tdapi.register_animation(name, frames)
end

function Animation:batch_add(animations)
   for name, frames in pairs(animations) do
	  tdapi.register_animation(name, frames)
   end
end

function Animation:begin(name)
   self.current = name
end

function Animation:advance_frame()
   local frames = tdapi.get_frames(self.current)
   self.frame = (self.frame + 1) % #frames
   self.time_to_next = DEFAULT_FRAME_TIME
end
