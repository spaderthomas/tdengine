local Default_Components = require('default_components')

entity.wilson.Graphic_Component = {
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

entity.wilson.Position_Component = {}

entity.wilson.Collision_Component = {
   kind = Collider_Kind.DYNAMIC,
   bounding_box = Default_Components.Human_Bounding_Box,
   on_collide = function(this, other) end
}

entity.wilson.Movement_Component = Default_Components.Human_Movement_Component

entity.wilson.Vision = {
   extents = {
	  width = px_x_to_screen(8),
	  depth = px_y_to_screen(32)
   } 
}

entity.wilson.Interaction_Component = {
   on_interact = function(this, other) end
}

entity.wilson.Task_Component = {}
