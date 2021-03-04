local inspect = require('inspect')
local GLFW = require('glfw')

Oliver = tdengine.entity('Oliver')
function Oliver:init(params)
   tdengine.register_collider(self.id)
end

function Oliver:update(dt)

end
