local class = require('middleclass')

Position = class('Position')
function Position:init()
   print('Position:init()')
   self.x = 0
   self.y = 0
end

function Position:update()
   print('Position:update()')
end
