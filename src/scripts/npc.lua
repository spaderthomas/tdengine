npc = {
    wilson = {
        State_Component = {
            states = {
			   "waiting_in_library",
			   "waiting_in_bar"
            },
            default_state = "waiting_in_library",
            watched_variables = {
			   "begin_cantina",
			   "talked_to_wilson_in_library"
            },
            update = function(state_component, updated_variable, value)
                if updated_variable == "begin_cantina" then
                    if value == true then
                        state_component:set_state("waiting_in_library")
                    end
				elseif updated_variable == "talked_to_wilson_in_library" then
				   if value == true then
					  state_component:set_state("waiting_in_bar")
				   end
                end
            end 
        },
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
        Movement_Component = {},
        Vision = {
            extents = {
                width = px_x_to_screen(8),
                depth = px_y_to_screen(32)
            } 
        },
		
        Interaction_Component = {
		   on_interact = function(this, other)
			  begin_dialogue(this)
		   end
        },
    },

    elia = {
        Graphic_Component = {
            Animations = {
                    stand = {
                        "elia_stand.png",
                    },
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
        Movement_Component = {},
        Vision = {
            extents = {
                width = px_x_to_screen(8),
                depth = px_y_to_screen(32)
            } 
        },
        dialogue = {
		   intro = {
			  text = "Try starting with hello!",
			  terminal = false;
			  responses = {
				 "press 1",
				 "press 2"
			  },
			  children = {
				 {
					text = "Uh..thank-thank you for...that...[nervous laughter] [more nervous laughter] [giggling] [faintly in the background, a hispanic man, yelling] JOEY JOEY and..end scene...",
					terminal = true;
					responses = {},
					children = {},
				 },
				 {
					text = "<an image of a fox appears onscreen>",
					terminal = true;
					responses = {},
					children = {},
				 },
			  }
		   }
		},
        Interaction_Component = {
		   on_interact = function(this, other)
			  begin_dialogue(this)
		   end
        },
    },

    rich = {
        Graphic_Component = {
            Animations = {
                    stand = {
                        "rich_stand.png",
                    },
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
        Movement_Component = {},
        Vision = {
            extents = {
                width = px_x_to_screen(8),
                depth = px_y_to_screen(32)
            } 
        },
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("Boon! It's so good to see you. Glad I got good news for you for once.")
            end
        }
    },
    
    sewell = {
        Graphic_Component = {
            Animations = {
                    stand = {
                        "sewell_stand.png",
                    },
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
        Movement_Component = {},
        Vision = {
            extents = {
                width = px_x_to_screen(8),
                depth = px_y_to_screen(32)
            } 
        },
        Interaction_Component = {
		   on_interact = function(this, other) 
			  show_text("(He looks meek, but not intimidated). So...you're Boon, huh? I kind of expected you to be taller.")
		   end
        }
    },

	intro_police = {
	   Graphic_Component = {
            Animations = {
                    stand = {
                        "intro_police_stand.png",
                    },
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
        Movement_Component = {},
        Interaction_Component = {
		   on_interact = function(this, other)
			  begin_dialogue(this)
		   end
        }
	},
}
