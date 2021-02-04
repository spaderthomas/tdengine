local inspect = require('inspect')

Graphic = tdengine.component('Graphic')
function Graphic:load(data)
   self.layer = data.layer or 1
   self.flags = data.flags or 0
end

function Graphic:init()
end

function Graphic:update(dt)
   tdengine.draw_entity(self.parent:get_id())
end
