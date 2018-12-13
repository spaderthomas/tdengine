local class = require('middleclass')
local inspect = require('inspect')
local GLFW = require('glfw')

Game = class('Game')
function Game:initialize()
   self.level = get_level("overworld")
   self.hero = create_entity("boon")
   camera_follow(self.hero)
end

function Game:update(dt)
	  -- Check for collisions among PC and NPCs
	  for i = 1, #self.level.entities do
		 local other = self.level.entities[i]
		 register_potential_collision(self.hero, other)
	  end

	  -- Check for interactions between hero and NPCs
	  if was_pressed(GLFW.KEY_E) then
		 for i = 1, #self.level.entities do
			local other = self.level.entities[i]
			if are_interacting(self.hero, other) then
			   set_interaction(self.hero, other)
			end
		 end
	  end
	  
	  -- Check for collisions among NPCs
	  for i = 1, #self.level.entities do
		 local this = self.level.entities[i]
			for j = i + 1, #self.level.entities do
			   local other = self.level.entities[j]
				  register_potential_collision(this, other)
			end
	  end

	  update_entity(self.hero, dt)

	  draw_entity(self.hero, Render_Flags.NO_FLAGS)
	  self.level:draw()
end

meta.after_load.game = {}
meta.after_load.game = function()
   game = Game:new()
end
