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
   self.hero = create_entity("boon")
   camera_follow(self.hero)
end

function Game:update(dt)
   if (self.state == GameState.GAME) then

	  -- Check for interactions
	  draw_entity(self.hero, Render_Flags.NO_FLAGS)
	  
	  -- Check for collisions
	  for i = 1, self.level:count_entities() do
		 local this = self.level.entities[i]
		 local collider = collider_kind(this)

		 -- Only check collisions for dynamic objects
		 if (collider == Collider_Kind.DYNAMIC) then
			for j = i + 1, self.level:count_entities() do
			   local other = self.level.entities[j]
			   local collider = collider_kind(other)

			   -- A dynamic collider can collide with anything
			   if (collider ~= Collider_Kind.NONE) then
				  register_potential_collision(this, other)
			   end
			end
		 end
	  end

	  update_entity(self.hero, dt)

	  self.level:draw()
	  
   end
end

meta.game = {}
meta.game.after_load = function()
   game = Game:new()
end
