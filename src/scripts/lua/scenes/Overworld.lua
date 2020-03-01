local inspect = require('inspect')

Overworld = tdapi.scene('Overworld')
function Overworld:init()
  print('Overworld:init()')
  self:add_entity('Spader')
end

function Overworld:update()
  --self:add_entity('Overworld', 'Spader')
  --print('Overworld:update()')
  --print(inspect(self))
end
