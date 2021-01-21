local inspect = require('inspect')
local GLFW = require('glfw')

Oliver = tdengine.entity('Oliver')
function Oliver:init()
   tdengine.register_collider(self:get_id())
end

function Oliver:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw()
end
