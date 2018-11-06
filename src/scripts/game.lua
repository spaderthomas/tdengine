local class = require('middleclass')
local inspect = require('inspect')

local GameState = {
   GAME = 0,
   DIALOGUE = 1
}

Game = class('Game')
Game.static.default_level = "overworld"
function Game:initialize()
   self.state = GameState.GAME
   self.level = Level.new()
   self.level.name = Game.default_level
   print("Loading level: " .. self.level.name)
   self.level:load()
end

function Game:update(dt)
   if (self.state == GameState.GAME) then
	  -- Check for collisions
	  for i = 1, self.level:count_entities() do
		 for j = 1, self.level:count_entities() do
			if (are_colliding(self.level.entities[i], self.level.entities[j])) then
			   on_collide(self.level.entities[i], self.level.entities[j])
			end
		 end
	  end

	  
   end
end

game = Game:new()
