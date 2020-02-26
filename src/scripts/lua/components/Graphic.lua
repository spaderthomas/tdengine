Graphic = tdapi.component('Graphic')
function Graphic:init()
   self.scale = { x = 0, y = 0 }
   self.layer = 0

   self.parent:add_component("Animation")
end


function Graphic:update()
end
