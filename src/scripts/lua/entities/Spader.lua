local inspect = require('inspect')
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

Spader = tdengine.entity('Spader')
function Spader:init()
  print(self:__tostring())
  local graphic = self:add_component("Graphic")
  graphic.scale = { x = .1, y = .1 }
  
  local animation = self:add_component("Animation")
  animation:batch_add(animations)
  animation:begin("spader_walk_down")
  
  local position = self:add_component("Position")

  local position = self:get_component("Position")
  print(position.world.x)
  self.value = "i'm a value"
end

function Spader:do_stuff()
  print('do_stuff')
  --print(self.value)
end

function Spader:update(dt)
  --self:do_stuff()
end
