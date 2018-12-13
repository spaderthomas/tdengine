local Default_Components = require('default_components')

entity.intro_police.Graphic_Component = {
   Animations = {
	  stand = {
		 "intro_police_stand.png",
	  },
   },
   default_animation = "stand",
   z = 1
}

entity.intro_police.Position_Component = {}

entity.intro_police.Collision_Component = {
   kind = Collider_Kind.DYNAMIC,
   bounding_box = Default_Components.Human_Bounding_Box,
   on_collide = function(this, other) end
}

entity.intro_police.Movement_Component = Default_Components.Human_Movement_Component

entity.intro_police.Interaction_Component = {
   on_interact = function(this, other) end
}

entity.intro_police.Action_Component = {
   update = function(this, dt) end
}
