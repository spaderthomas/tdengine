props = {
   door = {
        Bounding_Box = {
           center = {
              x = tile_x_to_screen(0),
              y = 0
           },
           extents = {
              x = tile_x_to_screen(2),
              y = tile_y_to_screen(2)
           }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("You ran into a door!")
            end
        },
		Door_Component = {},
		Collision_Component = {
		   on_collide = function(this, other)
			  any = this:get_component("Door_Component")
			  to = any.door.to
			  go_through_door(to)
		   end
		}
   },
   
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
        Bounding_Box = {
           center = {
              x = tile_x_to_screen(-1),
              y = 0
           },
           extents = {
              x = tile_x_to_screen(6),
              y = tile_y_to_screen(2)
           }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("It's just a barn, but in the future everything is grey.")
            end
        }
    },
    
    tree = {
       Graphic_Component = {
          Animations = {
             tree = {
                "tree.png",
             }
          },
          default_animation = "tree",
          z = 1,
        },
        Bounding_Box = {
           center = {
              x = 0,
              y = tile_y_to_screen(-1),
           },
           extents = {
              x = tile_x_to_screen(3),
              y = tile_y_to_screen(3)
           }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("Somehow, this tree is barely taller than you are.")
            end
        }
    },
    
    fence_left = {
        Graphic_Component = {
           Animations = {
              fence_left = {
                 "fence_left.png",
              }
           },
           default_animation = "fence_left",
           z = 1,
        },
        Bounding_Box = {
           center = {
              x = 0,
              y = tile_y_to_screen(-.5)
           },
           extents = {
              x = tile_x_to_screen(2),
              y = tile_y_to_screen(1)
           }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                wilson:say()
                show_text("It's just a fence.")
            end
        }
	},
	
     fence_middle = {
        Graphic_Component = {
           Animations = {
              fence_middle = {
                 "fence_middle.png",
              }
           },
           default_animation = "fence_middle",
           z = 1,
        },
        Bounding_Box = {
            center = {
               x = 0,
               y = tile_y_to_screen(-.5)
            },
            extents = {
                x = tile_x_to_screen(2),
                y = tile_y_to_screen(1)
            }
         },
         Position_Component = {},
         Interaction_Component = {
            on_interact = function(this, other) 
                show_text("It's just a fence.")
            end
        }
     },
	 
     fence_right = {
        Graphic_Component = {
           Animations = {
              fence_right = {
                 "fence_right.png",
              }
           },
           default_animation = "fence_right",
           z = 1,
        },
        Bounding_Box = {
            center = {
               x = 0,
               y = tile_y_to_screen(-.5)
            },
            extents = {
                x = tile_x_to_screen(2),
                y = tile_y_to_screen(1)
            }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("It's just a fence.")
            end
        }
     },
	 
     fence_post = {
        Graphic_Component = {
           Animations = {
              fence_post = {
                 "fence_post.png",
              }
           },
           default_animation = "fence_post",
           z = 1,
        },
        Bounding_Box = {
            center = {
               x = 0,
               y = tile_y_to_screen(-.5)
            },
            extents = {
                x = tile_x_to_screen(.5),
                y = tile_y_to_screen(1)
            }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("It's just a fence.")
            end
        }
     },
	 
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
        Bounding_Box = {
            center = {
               x = 0,
               y = 0
            },
            extents = {
                x = tile_x_to_screen(1),
                y = tile_y_to_screen(2)
            }
         },
         Position_Component = {},
         Interaction_Component = {
            on_interact = function(this, other) 
                show_text("It's just a fence.")
            end
        }
    },
    
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
        Position_Component = {}
    },
    
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
    },
    
    table_ = {
        Graphic_Component = {
            Animations = {
                table_ = {
                    "table_.png"
                }
            },
            default_animation = "table_",
            z = 1
        },
        Bounding_Box = {
           center = {
              x = 0,
              y = 0,
           },
           extents = {
              x = tile_x_to_screen(2),
              y = tile_y_to_screen(1)
           }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("Just thinking of all the things you could rest on this table fills you with excitement! What a rush!")
            end
        }
    },
    
    chair_left = {
        Graphic_Component = {
            Animations = {
                chair = {
                    "chair_left.png"
                }
            },
            default_animation = "chair",
            z = 1
        },
        Bounding_Box = {
           center = {
              x = 0,
              y = 0,
           },
           extents = {
            x = tile_x_to_screen(1),
            y = tile_y_to_screen(1)
           }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("This chair is made of a beautiful maple!")
            end
        }
    },
	
    chair_right = {
        Graphic_Component = {
            Animations = {
                chair = {
                    "chair_right.png"
                }
            },
            default_animation = "chair",
            z = 1
        },
        Bounding_Box = {
           center = {
              x = 0,
              y = 0,
           },
           extents = {
            x = tile_x_to_screen(1),
            y = tile_y_to_screen(1)
           }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("This chair is made of a beautiful maple!")
            end
        }
    },
	
    bar = {
        Graphic_Component = {
            Animations = {
                bar = {
                    "bar.png"
                }
            },
            default_animation = "bar",
            z = 1
        },
        Bounding_Box = {
           center = {
              x = 0,
              y = 0,
           },
           extents = {
              x = tile_x_to_screen(6.5),
              y = tile_y_to_screen(3.5)
           }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("Something feels off -- it feels like you're viewing this bar from above!")
            end
        }
    },
	
    liquor_cabinet = {
        Graphic_Component = {
            Animations = {
                liquor_cabinet = {
                    "liquor_cabinet.png"
                }
            },
            default_animation = "liquor_cabinet",
            z = 1
        },
        Bounding_Box = {
           center = {
              x = 0,
              y = 0,
           },
           extents = {
              x = tile_x_to_screen(6),
              y = tile_y_to_screen(2)
           }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("You see a fine potato liquer labelled Silver Gander.")
            end
        }
    },
	
    bookcase = {
        Graphic_Component = {
            Animations = {
                bookcase = {
                    "bookcase.png"
                }
            },
            default_animation = "bookcase",
            z = 1
        },
        Bounding_Box = {
           center = {
              x = 0,
              y = tile_y_to_screen(-.5),
           },
           extents = {
              x = tile_x_to_screen(2),
              y = tile_y_to_screen(1.5)
           }
        },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("It's just lined with copies of Infinite Jest...")
            end
        }
    },
	
    picture = {
        Graphic_Component = {
            Animations = {
                picture = {
                    "picture.png"
                }
            },
            default_animation = "picture",
            z = 1
        },
        Bounding_Box = {
            center = {
               x = 0,
               y = tile_y_to_screen(0),
            },
            extents = {
               x = tile_x_to_screen(1),
               y = tile_y_to_screen(.75)
            }
         },
        Position_Component = {},
        Interaction_Component = {
            on_interact = function(this, other) 
                show_text("It's a picture of two dripping hearts, joined together. You aren't sure if you like it or not.")
            end
        }
    }
}
