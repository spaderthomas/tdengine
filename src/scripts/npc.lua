npc = {
    wilson = {
        say = function(this) 
            print(this.current_state)
            if this.State_Component.current_state == "waiting_for_boon_first_aug" then
                show_text("Boon! Go to the bar and get your first augmentation!")
            elseif self.State_Component.current_state == "boon_has_first_aug" then
                show_text("Alright, now that that's taken care of, here is the rest of the plot")
            end
        end,
        State_Component = {
            states = {
                "waiting_in_library"
            },
            default_state = "waiting_in_library",
            current_state = "",
            watched_variables = {
                "begin_cantina"
            },
            update = function(this, updated_variable, value)
                if updated_variable == "begin_cantina" then
                    if value == true then
                        set_state(this, "waiting_in_library")
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
                self.say(this)
            end
        }
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
        Interaction_Component = {
            dialogue_fsm = {
                states = {
                    "waiting_for_boon_first_aug",
                    "boon_has_first_aug"
                },
                default_state = "boon_has_first_aug",
                current_state = "waiting_for_boon_first_aug",
                watched_variables = {
                    "boon_has_first_aug"
                },
                update = function(self, updated_variable, value)
                    if updated_variable == "boon_has_first_aug" then
                        if value == true then
                            self.current_state = "boon_has_first_aug"
                        end
                    end
                end
            },
            say = function(self) 
                show_text("... (He does not look up from his computer as he takes a sip of mead)")
            end,
            on_interact = function(self, this, other) 
                self:say()
            end
        }
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
            dialogue_fsm = {
                states = {
                    "waiting_for_boon_first_aug",
                    "boon_has_first_aug"
                },
                default_state = "boon_has_first_aug",
                current_state = "waiting_for_boon_first_aug",
                watched_variables = {
                    "boon_has_first_aug"
                },
                update = function(self, updated_variable, value)
                    if updated_variable == "boon_has_first_aug" then
                        if value == true then
                            self.current_state = "boon_has_first_aug"
                        end
                    end
                end
            },
            say = function(self) 
                show_text("Boon! It's so good to see you. Glad I got good news for you for once.")
            end,
            on_interact = function(self, this, other) 
                self:say()
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
            dialogue_fsm = {
                states = {},
                default_state = "",
                current_state = "",
                watched_variables = {},
                update = function(self, updated_variable, value)
                end
            },
            say = function(self) 
                show_text("(He looks meek, but not intimidated). So...you're Boon, huh? I kind of expected you to be taller.")
            end,
            on_interact = function(self, this, other) 
                self:say()
            end
        }
    }
}
