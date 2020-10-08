Physics = tdengine.component('Physics')
function Physics:load(data)
end

function Physics:init()
   self.had_collision = false
   self.collided_with = 0
end

function Physics:update()
end
