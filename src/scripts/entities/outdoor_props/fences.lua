entity.fence_left = {
    Graphic_Component = {
       Animations = {
          fence_left = {
             "fence_left.png",
          }
       },
       default_animation = "fence_left",
       z = 1,
    },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = tile_y_to_screen(-.5)
		  },
		  extents = {
			 x = tile_x_to_screen(2),
			 y = tile_y_to_screen(1)
		  }
	   },
	   on_collide = function(this, other) end
    },
    Position_Component = {},
    Interaction_Component = {
        on_interact = function(this, other) 
            wilson:say()
            show_text("It's just a fence.")
        end
    }
}

entity.fence_middle = {
   Graphic_Component = {
      Animations = {
         fence_middle = {
            "fence_middle.png",
         }
      },
      default_animation = "fence_middle",
      z = 1,
   },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = tile_y_to_screen(-.5)
		  },
		  extents = {
			 x = tile_x_to_screen(2),
			 y = tile_y_to_screen(1)
		  }
	   },
	   on_collide = function(this, other) end
    },
    Position_Component = {},
    Interaction_Component = {
       on_interact = function(this, other) 
           show_text("It's just a fence.")
       end
   }
}

entity.fence_right = {
   Graphic_Component = {
      Animations = {
         fence_right = {
            "fence_right.png",
         }
      },
      default_animation = "fence_right",
      z = 1,
   },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = tile_y_to_screen(-.5)
		  },
		  extents = {
			 x = tile_x_to_screen(2),
			 y = tile_y_to_screen(1)
		  }
	   },
	   on_collide = function(this, other) end
   },
   Position_Component = {},
   Interaction_Component = {
       on_interact = function(this, other) 
           show_text("It's just a fence.")
       end
   }
}

entity.fence_post = {
   Graphic_Component = {
      Animations = {
         fence_post = {
            "fence_post.png",
         }
      },
      default_animation = "fence_post",
      z = 1,
   },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = tile_y_to_screen(-.5)
		  },
		  extents = {
			 x = tile_x_to_screen(.5),
			 y = tile_y_to_screen(1)
		  }
	   },
	   on_collide = function(this, other) end
   },
   Position_Component = {},
   Interaction_Component = {
       on_interact = function(this, other) 
           show_text("It's just a fence.")
       end
   }
}
 
fence_top = {
    Graphic_Component = {
        Animations = {
            fence_top = {
                "fence_top.png"
            }
        },
        default_animation = "fence_top",
        z = 1
    },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = 0
		  },
		  extents = {
			 x = tile_x_to_screen(1),
			 y = tile_y_to_screen(2)
		  }
	   },
	   on_collide = function(this, other) end
     },
     Position_Component = {},
     Interaction_Component = {
        on_interact = function(this, other) 
            show_text("It's just a fence.")
        end
    }
}
