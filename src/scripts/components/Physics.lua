Physics = tdengine.component('Physics')
function Physics:init()
   self.parent:add_component('Position')
   self.parent:add_component('BoundingBox')

   tdengine.internal.register_collider(self.parent:get_id())

   self.had_collision = false
   self.collided_with = 0
end

function Physics:update()
end
