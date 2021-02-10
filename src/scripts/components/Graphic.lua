local inspect = require('inspect')

Graphic = tdengine.component('Graphic')
function Graphic:load(data)
   self.layer = data.layer or 1
   self.flags = data.flags or 0
   self.draw  = data.draw or true
end

function Graphic:init()
end

function Graphic:update(dt)
   if self.draw then tdengine.draw.entity(self.parent:get_id()) end
end

function Graphic:hide()
   self.draw = false
end

function Graphic:show()
   self.draw = true
end
