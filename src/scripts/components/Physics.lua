Physics = tdengine.component('Physics')
function Physics:init(params)
   self.had_collision = false
   self.collided_with = 0
end

function Physics:late_init()
end

function Physics:update()
end
