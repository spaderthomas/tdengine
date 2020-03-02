local inspect = require('inspect')

Overworld = tdengine.scene('Overworld')
function Overworld:init()
  print('Overworld:init()')
  self:add_entity('Spader')
end

function Overworld:update()
  --self:add_entity('Overworld', 'Spader')
  --print('Overworld:update()')
  --print(inspect(self))
end
