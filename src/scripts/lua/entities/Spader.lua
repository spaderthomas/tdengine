local class = require('middleclass')

Spader = class('Spader')
function Spader:init()
   print('Spader:init()')
   print('ID: ' .. self:get_id())
   print('Name: ' .. self:get_name())

   local graphic = self:add_component("Graphic")
   graphic.scale = { x = .1, y = .1 }
   
   local animation = self:add_component("Animation")
   animation:add("SpaderStand", { "spader_stand_1.png" })
   animation:begin("SpaderStand")
   
   local position = self:add_component("Position")
end

function Spader:update(dt)
   print('Spader:update()')
   self:do_stuff()
end

function Spader:do_stuff()
   print('Spader::do_stuff()')
end
