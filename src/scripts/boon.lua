boon = {
    boon = {
        Graphic_Component = {
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
         },
         Position_Component = {
            pos = {
                x = .25,
                y = .25,
            },
            scale = {
                x = 32,
                y = 32
            }
        },
        Bounding_Box = {
            center = {
               x = 0,
               y = tile_y_to_screen(-.5)
            },
            extents = {
               x = tile_x_to_screen(1.25),
               y = tile_y_to_screen(.5)
            }
         },
        Movement_Component = {
        },
        Vision = {
            extents = {
                width = px_x_to_screen(8),
                depth = px_y_to_screen(32)
             } 
        }
    }
}
