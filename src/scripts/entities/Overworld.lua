local inspect = require('inspect')

Overworld = tdengine.entity('Overworld')
function Overworld:init()
   self:create_entity('Player')
end

function Overworld:update()
end
