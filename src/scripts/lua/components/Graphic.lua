local class = require('middleclass')
local inspect = require('inspect')

Graphic = class('Graphic')
function Graphic:init()
   self.scale = { x = 0, y = 0 }
   self.layer = 0
   self.animations = {}

   self.parent:add_component("Animation")
end

function Graphic:update()
   
end
