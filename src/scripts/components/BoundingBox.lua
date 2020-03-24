BoundingBox = tdengine.component('BoundingBox')
function BoundingBox:init()
   local position = self.parent:get_component('Position')
   self.origin = position.world
   self.extents = { x = .1, y = .1 }
end

function BoundingBox:update()
end
