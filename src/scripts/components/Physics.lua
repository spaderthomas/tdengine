Physics = tdengine.component('Physics')
function Physics:init()
   self.parent:add_component('Position')
   self.parent:add_component('BoundingBox')

   tdengine.internal.register_collider(self.parent:get_id())
end

function Physics:update()
end
