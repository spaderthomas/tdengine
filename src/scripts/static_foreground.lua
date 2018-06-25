tree = {
   Graphic_Component = {
	  Animations = {
		 tree = {
			"tree1.png",
		 }
	  },
	  default_animation = "tree",
      z = 1,
    },
    Collision_Component = {
        top = .5,
        bottom = 0,
        left = 0, 
        right = 1
    },
    Position_Component = {
        translate = {
            x = 0,
            y = 0,
            z = 0
        },
        scale = {
            x = 64,
            y = 64
        },
        rad_rot = 0
    }
}

fence = {
	Graphic_Component = {
	   Animations = {
		  fence = {
			 "fence.png",
		  }
	   },
	   default_animation = "fence",
	   z = 1,
	},
    Collision_Component = {
        top = .5,
        bottom = 0,
        left = 0, 
        right = 1
    },
    Position_Component = {
        translate = {
            x = 0,
            y = 0,
            z = 0
        },
        scale = {
            x = 32,
            y = 32
        },
        rad_rot = 0
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
        top = .5,
        bottom = 0,
        left = 0, 
        right = 1
    },
    Position_Component = {
        translate = {
            x = 0,
            y = 0,
            z = 0
        },
        scale = {
            x = 32,
            y = 64
        },
        rad_rot = 0
    }
}

 cactus = {
    Graphic_Component = {
        Animations = {
            cactus = {
                "cactus.png"
            }
        },
        default_animation = "cactus",
        z = 1
    },
    Collision_Component = {
        top = .5,
        bottom = 0,
        left = 0, 
        right = 1
    },
    Position_Component = {
        translate = {
            x = 0,
            y = 0,
            z = 0
        },
        scale = {
            x = 32,
            y = 32
        },
        rad_rot = 0
    }
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
    Collision_Component = {
        top = .5,
        bottom = 0,
        left = 0, 
        right = 1
    },
    Position_Component = {
        translate = {
            x = 0,
            y = 0,
            z = 0
        },
        scale = {
            x = 16,
            y = 16
        },
        rad_rot = 0
    }
}

building = {
    Graphic_Component = {
        Animations = {
            building = {
                "building.png"
            }
        },
        default_animation = "building",
        z = 1
    },
    Collision_Component = {
        top = .5,
        bottom = 0,
        left = 0, 
        right = 1
    },
    Position_Component = {
        translate = {
            x = 0,
            y = 0,
            z = 0
        },
        scale = {
            x = 128,
            y = 128
        },
        rad_rot = 0
    }
} 

