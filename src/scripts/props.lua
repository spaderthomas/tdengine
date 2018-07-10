props = {
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
        Position_Component = {}
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
        Position_Component = {}
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
        Position_Component = {}
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
         Position_Component = {}
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
        Position_Component = {}
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
        Position_Component = {}
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
         Position_Component = {}
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
        Position_Component = {}
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
        Position_Component = {}
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
        Position_Component = {}
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
        Position_Component = {}
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
        Position_Component = {}
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
        Position_Component = {}
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
        Position_Component = {}
    }
}
