local class = require('middleclass')

Animation = class('Animation')
function Animation:init()
   self.animations = {}
   self.current = ""
   self.frame = 0

   print("Animation:init()");
end

function Animation:update()
   print("Animation:update()");
end

function Animation:add(name, frames)
   tdapi.register_animation(name, frames)
end

function Animation:begin(name)
   self.current = name
end
