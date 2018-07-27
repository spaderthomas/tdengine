wilson = {
    wilson = {
        Graphic_Component = {
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
         },
        Position_Component = {},
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
        },
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("Boon. You have to listen. You need to put more text!")
                print(this)
            end
        }
    }
}
