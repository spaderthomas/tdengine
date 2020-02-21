local class = require('middleclass')

GraphicComponent = class('GraphicComponent')
function GraphicComponent:init()
   self.scale = { x = 0, y = 0 }
   self.layer = 0
   self.animations = {}
   self.animation = ""

end

function GraphicComponent:update()
   
end
