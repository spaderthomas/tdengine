local class = require('middleclass')

Spader = class('Spader')
function Spader:init()
   print('Spader:init()')
   print('ID: ' .. self:get_id())
   print('Name: ' .. self:get_name())

   local graphic = self:add_component("Graphic")
   local position = self:add_component("Position")
   print('Position: ' .. position.world.x .. ', ' .. position.world.y)
end

function Spader:update(dt)
   print('Spader:update()')
   self:do_stuff()
end

function Spader:do_stuff()
   print('Spader::do_stuff()')
end
