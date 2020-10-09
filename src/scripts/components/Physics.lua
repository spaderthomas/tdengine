Physics = tdengine.component('Physics')
function Physics:load(data)
end

function Physics:init()
   tdengine.internal.register_collider(self.parent:get_id())

   self.had_collision = false
   self.collided_with = 0
end

function Physics:update()
end
