boon = {
   Graphic_Component = {
        Animations = {
            walk = {
                "boon_walk1.png",
                "boon_walk2.png",
                "boon_walk3.png",
                "boon_walk4.png",
                "boon_walk5.png"
            }
        },
        default_animation = "walk",
        z = .5
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
	Collision_Component = {
        center = {
           x = 0,
           y = 0
        },
        extents = {
           x = tile_x_to_screen(1.25),
           y = tile_y_to_screen(1.75)
        }
     },
     Movement_Component = {
     },
}
