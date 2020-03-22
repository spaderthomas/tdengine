Tom = tdengine.entity('Tom')
function Tom:init()
   print("I'm Tom!")
   local position = self:add_component('Position')
   position.x = 68
   position.y = 421
end

function Tom:update(dt)
   local position = self:get_component('Position')
   print("I'm still Tom! And I'm located at (" .. position.x .. ', ' .. position.y .. ')')
end
