entity.tree = {
   Graphic_Component = {
      Animations = {
         tree = {
            "tree.png",
         }
      },
      default_animation = "tree",
      z = 1,
   },
   Collision_Component = {
	  kind = Collider_Kind.STATIC,
	  bounding_box = {
		 center = {
			x = 0,
			y = tile_y_to_screen(-1),
		 },
		 extents = {
			x = tile_x_to_screen(3),
			y = tile_y_to_screen(3)
		 }
	  },
	  on_collide = function(this, other) end
    },
    Position_Component = {},
    Interaction_Component = {
        on_interact = function(this, other) 
            show_text("Somehow, this tree is barely taller than you are.")
        end
    }
}

entity.cactus = {
    Graphic_Component = {
        Animations = {
            cactus = {
                "cactus.png"
            }
        },
        default_animation = "cactus",
        z = 1
    },
    Position_Component = {}
}

bush = {
    Graphic_Component = {
        Animations = {
            bush = {
                "bush.png"
            }
        },
        default_animation = "bush",
        z = 1
    },
    Position_Component = {}
}
