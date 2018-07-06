building = {
    Graphic_Component = {
        Animations = {
            building = {
                "building.png"
            }
        },
        default_animation = "building",
        z = .5
    },
    Collision_Component = {
	   center = {
		  x = 0,
		  y = 0
	   },
	   extents = {
		  x = tile_x_to_screen(3),
		  y = tile_y_to_screen(3)
	   }
	},
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 128,
            y = 128
        }
    }
} 

tree = {
   Graphic_Component = {
	  Animations = {
		 tree = {
			"tree.png",
		 }
	  },
	  default_animation = "tree",
      z = .5,
    },
    Collision_Component = {
	   center = {
		  x = 0,
		  y = 0,
	   },
	   extents = {
		  x = 32 / 680,
		  y = 32 / 360
	   }
	},
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 64,
            y = 64
        }
    }
}

fence_left = {
	Graphic_Component = {
	   Animations = {
		  fence_left = {
			 "fence_left.png",
		  }
	   },
	   default_animation = "fence_left",
	   z = .5,
	},
    Collision_Component = {
	   center = {
		  x = 0,
		  y = 0
	   },
	   extents = {
		  x = 32 / 680,
		  y = 32 / 360
	   }
	},
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 32,
            y = 32
        }
    }
 }
 fence_middle = {
	Graphic_Component = {
	   Animations = {
		  fence_middle = {
			 "fence_middle.png",
		  }
	   },
	   default_animation = "fence_middle",
	   z = .5,
	},
    Collision_Component = {
        center = {
           x = 0,
           y = 0
        },
        extents = {
           x = 32 / 680,
           y = 32 / 360
        }
     },
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 32,
            y = 32
        }
    }
 }
 fence_right = {
	Graphic_Component = {
	   Animations = {
		  fence_right = {
			 "fence_right.png",
		  }
	   },
	   default_animation = "fence_right",
	   z = .5,
	},
    Collision_Component = {
        center = {
           x = 0,
           y = 0
        },
        extents = {
           x = 32 / 680,
           y = 32 / 360
        }
     },
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 32,
            y = 32
        }
    }
 }
 fence_post = {
	Graphic_Component = {
	   Animations = {
		  fence_post = {
			 "fence_post.png",
		  }
	   },
	   default_animation = "fence_post",
	   z = .5,
	},
    Collision_Component = {
        center = {
           x = 0,
           y = 0
        },
        extents = {
           x = 32 / 680,
           y = 32 / 360
        }
     },
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 32,
            y = 32
        }
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
        z = .5
    },
    Collision_Component = {
        center = {
           x = 0,
           y = 0
        },
        extents = {
           x = 32 / 680,
           y = 32 / 360
        }
     },
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 32,
            y = 64
        }
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
        z = .5
    },
    Collision_Component = {
	   center = {
		  x = 0,
		  y = 0,
	   },
	   extents = {
		  x = 32 / 680,
		  y = 32 / 360
	   }
	},
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 32,
            y = 32
        }
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
        z = .5
    },
    Collision_Component = {
	   center = {
		  x = 0,
		  y = 0,
	   },
	   extents = {
		  x = 32 / 680,
		  y = 32 / 360
	   }
	},
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 16,
            y = 16
        }
    }
}

table_ = {
    Graphic_Component = {
        Animations = {
            table_ = {
                "table_.png"
            }
        },
        default_animation = "table_",
        z = .5
    },
    Collision_Component = {
	   center = {
		  x = 0,
		  y = 0,
	   },
	   extents = {
		  x = tile_x_to_screen(2),
		  y = tile_y_to_screen(1.5)
	   }
	},
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 32,
            y = 32
        }
    }
}

chair_left = {
    Graphic_Component = {
        Animations = {
            chair = {
                "chair_left.png"
            }
        },
        default_animation = "chair",
        z = .5
    },
    Collision_Component = {
	   center = {
		  x = 0,
		  y = 0,
	   },
	   extents = {
        x = tile_x_to_screen(1),
        y = tile_y_to_screen(1)
	   }
	},
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 32,
            y = 32
        }
    }
}
chair_right = {
    Graphic_Component = {
        Animations = {
            chair = {
                "chair_right.png"
            }
        },
        default_animation = "chair",
        z = .5
    },
    Collision_Component = {
	   center = {
		  x = 0,
		  y = 0,
	   },
	   extents = {
        x = tile_x_to_screen(1),
        y = tile_y_to_screen(1)
	   }
	},
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 32,
            y = 32
        }
    }
}
bar = {
    Graphic_Component = {
        Animations = {
            bar = {
                "bar.png"
            }
        },
        default_animation = "bar",
        z = .5
    },
    Collision_Component = {
	   center = {
		  x = 0,
		  y = 0,
	   },
	   extents = {
          x = tile_x_to_screen(8),
          y = tile_y_to_screen(4)
	   }
	},
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 128,
            y = 64  
        }
    }
}
liquor_cabinet = {
    Graphic_Component = {
        Animations = {
            liquor_cabinet = {
                "liquor_cabinet.png"
            }
        },
        default_animation = "liquor_cabinet",
        z = .5
    },
    Collision_Component = {
	   center = {
		  x = 0,
		  y = 0,
	   },
	   extents = {
          x = tile_x_to_screen(8),
          y = tile_y_to_screen(4)
	   }
	},
    Position_Component = {
        pos = {
            x = 0,
            y = 0
        },
        scale = {
            x = 128,
            y = 64  
        }
    }
}