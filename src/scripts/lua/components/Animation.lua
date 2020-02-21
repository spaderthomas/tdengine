local class = require('middleclass')

AnimationData = class('AnimationData')
function AnimationData:initialize()
   self.frames = {}
   self.frame = ""
end

Animation = class('Animation')
function Animation:init()
   print("Animation:init()");
end

function Animation:update()
   print("Animation:update()");
end
