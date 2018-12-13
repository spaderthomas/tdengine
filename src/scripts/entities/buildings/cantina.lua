entity.building = {
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
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = tile_x_to_screen(-1),
			 y = 0
		  },
		  extents = {
			 x = tile_x_to_screen(6),
			 y = tile_y_to_screen(2)
		  }
	   },
	   on_collide = function(this, other) end
    },
    Position_Component = {},
    Interaction_Component = {
        on_interact = function(this, other) 
            show_text("It's just a barn, but in the future everything is grey.")
        end
    }
}

entity.table_ = {
    Graphic_Component = {
        Animations = {
            table_ = {
                "table_.png"
            }
        },
        default_animation = "table_",
        z = 1
    },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = 0,
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
            show_text("Just thinking of all the things you could rest on this table fills you with excitement! What a rush!")
        end
    }
}

entity.chair_left = {
    Graphic_Component = {
        Animations = {
            chair = {
                "chair_left.png"
            }
        },
        default_animation = "chair",
        z = 1
    },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = 0,
		  },
		  extents = {
			 x = tile_x_to_screen(1),
			 y = tile_y_to_screen(1)
		  }
	   },
	   on_collide = function(this, other) end
    },
    Position_Component = {},
    Interaction_Component = {
        on_interact = function(this, other) 
            show_text("This chair is made of a beautiful maple!")
        end
    }
}

entity.chair_right = {
    Graphic_Component = {
        Animations = {
            chair = {
                "chair_right.png"
            }
        },
        default_animation = "chair",
        z = 1
    },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = 0,
		  },
		  extents = {
			 x = tile_x_to_screen(1),
			 y = tile_y_to_screen(1)
		  }
	   },
	   on_collide = function(this, other) end
    },
    Position_Component = {},
    Interaction_Component = {
        on_interact = function(this, other) 
            show_text("This chair is made of a beautiful maple!")
        end
    }
}

entity.bar = {
    Graphic_Component = {
        Animations = {
            bar = {
                "bar.png"
            }
        },
        default_animation = "bar",
        z = 1
    },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = 0,
		  },
		  extents = {
			 x = tile_x_to_screen(6.5),
			 y = tile_y_to_screen(3.5)
		  }
	   },
	   on_collide = function(this, other) end
    },
    Position_Component = {},
    Interaction_Component = {
        on_interact = function(this, other) 
            show_text("Something feels off -- it feels like you're viewing this bar from above!")
        end
    }
}

entity.liquor_cabinet = {
    Graphic_Component = {
        Animations = {
            liquor_cabinet = {
                "liquor_cabinet.png"
            }
        },
        default_animation = "liquor_cabinet",
        z = 1
    },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = 0,
		  },
		  extents = {
			 x = tile_x_to_screen(6),
			 y = tile_y_to_screen(2)
		  }
	   },
	   on_collide = function(this, other) end
    },
    Position_Component = {},
    Interaction_Component = {
        on_interact = function(this, other) 
            show_text("You see a fine potato liquer labelled Silver Gander.")
        end
    }
}

entity.bookcase = {
    Graphic_Component = {
        Animations = {
            bookcase = {
                "bookcase.png"
            }
        },
        default_animation = "bookcase",
        z = 1
    },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = tile_y_to_screen(-.5),
		  },
		  extents = {
			 x = tile_x_to_screen(2),
			 y = tile_y_to_screen(1.5)
		  }
	   },
	   on_collide = function(this, other) end
    },
    Position_Component = {},
    Interaction_Component = {
        on_interact = function(this, other) 
            show_text("It's just lined with copies of Infinite Jest...")
        end
    }
}

entity.picture = {
    Graphic_Component = {
        Animations = {
            picture = {
                "picture.png"
            }
        },
        default_animation = "picture",
        z = 1
    },
	Collision_Component = {
	   kind = Collider_Kind.STATIC,
	   bounding_box = {
		  center = {
			 x = 0,
			 y = tile_y_to_screen(0),
		  },
		  extents = {
			 x = tile_x_to_screen(1),
			 y = tile_y_to_screen(.75)
		  }
	   },
	   on_collide = function(this, other) end
     },
    Position_Component = {},
    Interaction_Component = {
        on_interact = function(this, other) 
            show_text("It's a picture of two dripping hearts, joined together. You aren't sure if you like it or not.")
        end
    }
}
