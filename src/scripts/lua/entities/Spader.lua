local class = require('middleclass')

local animations = {
   spader_stand = {
	  "spader_stand_1.png"
   },
   spader_walk_down = {
	  "spader_walk_down1.png",
	  "spader_walk_down2.png",
	  "spader_walk_down3.png",
	  "spader_walk_down4.png",
	  "spader_walk_down5.png",
	  "spader_walk_down6.png"
   }
}

Spader = class('Spader')
function Spader:init()
   local graphic = self:add_component("Graphic")
   graphic.scale = { x = .1, y = .1 }
   
   local animation = self:add_component("Animation")
   animation:batch_add(animations)
   animation:begin("spader_walk_down")
   
   local position = self:add_component("Position")
end

function do_stuff()
   print('dfasdfadsfadsfdasfasdfdasfadsfdasf')
end

function Spader:update(dt)
   do_stuff()
end


