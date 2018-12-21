-- Components
entity.boonhouse_door = {}
entity.boonhouse_door.Graphic_Component = {
    Animations = {
        boonhouse_door = {
            "boonhouse_door.png"
        }
    },
    default_animation = "boonhouse_door",
    z = 1
}

entity.boonhouse_door.Collision_Component = {
   kind = Collider_Kind.STATIC,
   bounding_box = {
	  center = {
		 x = 0,
		 y = 0
	  },
	  extents = {
		 x = tile_x_to_screen(1),
		 y = tile_y_to_screen(1)
	  }
   },
   on_collide = function(this, other) end
}

entity.boonhouse_door.Position_Component = {}

entity.boonhouse_door.Interaction_Component = {
    on_interact = function(this, other) end
}

entity.boonhouse_door.Task_Component = {}


-- Scripts
entity.boonhouse_door.scripts = {}

entity.boonhouse_door.scripts.intro_unchecked = {
   {
	  kind = "Wait_For_Interaction_Action",
	  is_blocking = true
   },
   {
	  kind = "Set_State_Action",
	  is_blocking = false,
	  var = "intro_door_answered",
	  value = true
   }
}

entity.boonhouse_door.scripts.intro_checked = {}


-- State machine
entity.boonhouse_door.State_Machine = {}
entity.boonhouse_door.State_Machine.intro_unchecked = {
   task = entity.boonhouse_door.scripts.intro_unchecked,
   transitions = {
	  {
		 vars = {
			intro_door_answered = true,
		 },
		 next_state = "intro_checked"
	  }
   }
}

entity.boonhouse_door.State_Machine.intro_checked = {
   task = entity.boonhouse_door.scripts.intro_unchecked,
   transitions = {}
}