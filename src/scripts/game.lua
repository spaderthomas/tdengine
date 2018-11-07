local class = require('middleclass')
local inspect = require('inspect')
local GLFW = require('glfw')

local GameState = {
   GAME = 0,
   DIALOGUE = 1
}

Game = class('Game')
Game.static.default_level = "overworld"
function Game:initialize()
   self.state = GameState.GAME
   self.level = get_level(Game.default_level)
end

function Game:update(dt)
   if (self.state == GameState.GAME) then
	  
	  -- Check for collisions
	  for i = 1, self.level:count_entities() do
		 local this = self.level.entities[i]
		 local collider = collider_kind(this)
		 if (collider == Collider_Kind.DYNAMIC) then
			print("Dynamic!")
		 end
	  end

	  
   end
end

game = nil
