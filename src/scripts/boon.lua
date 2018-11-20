local GLFW = require('glfw')
local Default_Components = require('default_components')

boon = {}

-- Boon
boon.boon = {}
boon.boon.Graphic_Component = {
   Animations = {
	  
	  stand = {
		 "boon_stand.png",
	  },
	  walk_down = {
		 "boon_walk_down1.png",
		 "boon_walk_down2.png",
		 "boon_walk_down3.png",
		 "boon_walk_down4.png",
		 "boon_walk_down5.png",
		 "boon_walk_down6.png"
	  },
	  walk_up = {
		 "boon_walk_up1.png",
		 "boon_walk_up2.png",
		 "boon_walk_up3.png",
		 "boon_walk_up4.png",
		 "boon_walk_up5.png",
		 "boon_walk_up6.png"
	  }
   },
   default_animation = "stand",
   z = 1
}

boon.boon.Position_Component = {}

boon.boon.Collision_Component = {
   kind = Collider_Kind.DYNAMIC,
   bounding_box = Default_Components.Human_Bounding_Box,
   on_collide = function(this, other) end
}

boon.boon.Movement_Component = Default_Components.Human_Movement_Component

boon.boon.Vision = {
   extents = {
	  width = px_x_to_screen(8),
	  depth = px_y_to_screen(32)
   } 
}

boon.boon.Update_Component = {
   update = function(this, dt)
	  if (is_down(GLFW.KEY_W)) then
		 move_entity(this, true, false, false, false)
		 set_animation_no_reset(this, "walk_up")
	  end
	  if (is_down(GLFW.KEY_A)) then
		 move_entity(this, false, false, true, false)
	  end
	  if (is_down(GLFW.KEY_S)) then
		 move_entity(this, false, true, false, false)
		 set_animation_no_reset(this, "walk_down")
	  end
	  if (is_down(GLFW.KEY_D)) then
		 move_entity(this, false, false, false, true)
	  end

	  update_animation(this, dt)
	  
   end
}
