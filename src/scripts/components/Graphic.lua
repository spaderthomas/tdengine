local inspect = require('inspect')

Graphic = tdengine.component('Graphic')
function Graphic:init(params)
   self.layer = params.layer or 1
   self.flags = params.flags or 0
   self.draw  = params.draw or true
end

function Graphic:update(dt)
  if self.draw then
	tdengine.draw.entity(self.parent.id)
  end
end

function Graphic:hide()
   self.draw = false
end

function Graphic:show()
   self.draw = true
end
