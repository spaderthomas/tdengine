local Default_Components = require('default_components')

npc = {}

-- Wilson
npc.wilson = {}
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
   bounding_box = Default_Components.Human_Bounding_Box,
   on_collide = function(this, other) end
}

npc.wilson.Movement_Component = Default_Components.Human_Movement_Component

npc.wilson.Vision = {
   extents = {
	  width = px_x_to_screen(8),
	  depth = px_y_to_screen(32)
   } 
}

npc.wilson.Interaction_Component = {
   on_interact = function(this, other) end
}

npc.wilson.Task_Component = {}

npc.wilson.scripts = {}
npc.wilson.scripts.intro = {
   {
	  kind = "And_Action",
	  actions = {
		 {
			kind = "Wait_For_Interaction_Action",
			is_blocking = false
		 },
		 {
			kind = "Movement_Action",
			is_blocking = false,
			dest = {
			   x = .4,
			   y = .4
			}
		 }
	  }
   },
   {
	  kind = "Dialogue_Action",
	  dialogue = {
		 text = "*intro dialogue*",
		 terminal = true,
		 responses = {
			"dialogue makes games fun lol"
		 },
		 children = {}
	  }
   }
}

npc.wilson.scripts.main1 = {
   {
	  kind = "Wait_For_Interaction_Action",
	  is_blocking = true
   },
   {
	  kind = "Dialogue_Action",
	  dialogue = {
		 text = "*main1 dialogue*",
		 terminal = true,
		 responses = {},
		 children = {}
	  }
   }
}

npc.wilson.State_Machine = {}
npc.wilson.State_Machine.intro = {
   task = npc.wilson.scripts.intro,
   transitions = {
	  {
		 vars = {
			a = true,
		 },
		 next_state = "main1"
	  }
   }
}

npc.wilson.State_Machine.main1 = {
   task = npc.wilson.scripts.main1,
   transitions = {}
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
   bounding_box = Default_Components.Human_Bounding_Box,
   on_collide = function(this, other) end
}

npc.intro2.Movement_Component = Default_Components.Human_Movement_Component

npc.intro2.Interaction_Component = {
   on_interact = function(this, other) end
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

npc.intro_police.Collision_Component = {
   kind = Collider_Kind.DYNAMIC,
   bounding_box = Default_Components.Human_Bounding_Box,
   on_collide = function(this, other) end
}

npc.intro_police.Movement_Component = Default_Components.Human_Movement_Component

npc.intro_police.Interaction_Component = {
   on_interact = function(this, other) end
}

npc.intro_police.Action_Component = {
   update = function(this, dt) end
}


