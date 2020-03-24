local inspect = require('inspect')

Overworld = tdengine.entity('Overworld')
function Overworld:init()
   self:create_entity('Spader')
   self:create_entity('Wilson')
end

function Overworld:update()
end
