local class = require('middleclass')

npc = {}

-- Wilson
npc.wilson = {}
npc.wilson.State_Component = {
   states = {
	  "waiting_in_library",
	  "waiting_in_bar"
   },
   default_state = "waiting_in_library",
   watched_variables = {
	  "begin_cantina",
	  "talked_to_wilson_in_library"
   },
   update = function(state_component, updated_variable, value)
	  if updated_variable == "begin_cantina" then
		 if value == true then
			state_component:set_state("waiting_in_library")
		 end
	  elseif updated_variable == "talked_to_wilson_in_library" then
		 if value == true then
			state_component:set_state("waiting_in_bar")
		 end
	  end
   end 
}

npc.wilson.Graphic_Component = {
   Animations = {
	  stand = {
		 "wilson_stand.png",
	  },
	  walk_down = {
		 "wilson_walk_down1.png",
		 "wilson_walk_down2.png",
		 "wilson_walk_down3.png",
		 "wilson_walk_down4.png",
		 "wilson_walk_down5.png",
		 "wilson_walk_down6.png"
	  }
   },
   default_animation = "stand",
   z = 1
}

npc.wilson.Position_Component = {}

npc.wilson.Collision_Component = {
   kind = Collider_Kind.DYNAMIC,
   bounding_box = {
	  center = {
		 x = 0,
		 y = tile_y_to_screen(-.5)
	  },
	  extents = {
		 x = tile_x_to_screen(1.25),
		 y = tile_y_to_screen(.5)
	  }
   },
   on_collide = function(this, other) end
}

npc.wilson.Movement_Component = {}

npc.wilson.Vision = {
   extents = {
	  width = px_x_to_screen(8),
	  depth = px_y_to_screen(32)
   } 
}

Interaction_Component = {
   on_interact = function(this, other)
	  begin_dialogue(this)
   end
}


-- Intro2
npc.intro2 = {}
npc.intro2.Graphic_Component = {
   Animations = {
	  stand = {
		 "intro1_stand.png"
	  }
   },
   default_animation = "stand",
   z = 1
}

npc.intro2.Position_Component = {}


npc.intro2.Collision_Component = {
   kind = Collider_Kind.DYNAMIC,
   bounding_box = {
	  center = {
		 x = 0,
		 y = tile_y_to_screen(-.5)
	  },
	  extents = {
		 x = tile_x_to_screen(1.25),
		 y = tile_y_to_screen(.5)
	  }
   },
   on_collide = function(this, other) end
}

npc.intro2.Movement_Component = {}

npc.intro2.Interaction_Component = {
   on_interact = function(this, other)
	  begin_dialogue(this)
   end
}


-- Intro Police
npc.intro_police = {}
npc.intro_police.Graphic_Component = {
   Animations = {
	  stand = {
		 "intro_police_stand.png",
	  },
   },
   default_animation = "stand",
   z = 1
}

npc.intro_police.Position_Component = {}

npc.intro_police.Bounding_Box = {
   center = {
	  x = 0,
	  y = tile_y_to_screen(-.5)
   },
   extents = {
	  x = tile_x_to_screen(1.25),
	  y = tile_y_to_screen(.5)
   }
}

npc.intro_police.Movement_Component = {}

npc.intro_police.Interaction_Component = {
   on_interact = function(this, other)
	  begin_dialogue(this)
   end
}

npc.intro_police.Action_Component = {
   update = function(this, dt)
	  
   end
}


