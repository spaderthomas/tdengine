local inspect = require('inspect')

Overworld = tdengine.entity('Overworld')
function Overworld:init()
   local tom = self:create_entity('Tom')
   local position = self:get_component('Position')
   print(inspect(tom))
   self:create_entity('Player')
end

function Overworld:update()
end
