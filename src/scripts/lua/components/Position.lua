local class = require('middleclass')

Position = class('Position')
function Position:init()
   print('Position:init()')
   self.world = {
	  x = 0
	  y = 0
   }
end

function Position:update()
   print('Position:update()')
end
