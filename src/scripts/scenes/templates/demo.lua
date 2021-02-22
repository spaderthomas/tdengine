return {
  entities = {
    {
      components = {
        Animation = {
          animations = {
            classroom_background = {
              "classroom.png"
            }
          },
          current = "classroom_background"
        },
        Position = {
          world = {
            x = 3,
            y = 3
          }
        }
      },
      name = "Background",
      params = {}
    },
    {
      components = {
        Position = {
          world = {
            x = 0.1954183429479599,
            y = 0.3807280957698822
          }
        }
      },
      name = "Player",
      params = {}
    },
    {
      components = {
        BoundingBox = {
          extents = {
            x = 0.10041841119527817,
            y = 0.1796116828918457
          },
          offset = {
            x = 0,
            y = 0
          }
        },
        Position = {
          world = {
            x = 0.050209205597639084,
            y = 0.90436893701553345
          }
        }
      },
      name = "Box",
      params = {}
    },
    {
      components = {
        BoundingBox = {
          extents = {
            x = 0.20031380653381348,
            y = 0.35436898469924927
          },
          offset = {
            x = 0,
            y = 0
          }
        },
        Position = {
          world = {
            x = 0.30047070980072021,
            y = 0.81504857540130615
          }
        }
      },
      name = "Box",
      params = {}
    },
    {
      components = {
        BoundingBox = {
          extents = {
            x = 0.099479168653488159,
            y = 0.17499999701976776
          },
          offset = {
            x = 0,
            y = 0
          }
        },
        Position = {
          world = {
            x = 0.050260379910469055,
            y = 0.090648181736469269
          }
        }
      },
      name = "Door",
      params = {
        where = "intro"
      }
    },
    {
	   components = {
        BoundingBox = {
          extents = {
            x = 0.099479168653488159,
            y = 0.17499999701976776
          },
          offset = {
            x = 0,
            y = 0
          }
        },		  
        Position = {
          world = {
            x = 0.250260379910469055,
            y = 0.290648181736469269
          }
        }
      },
      name = "Interactable",
	  params = {
		 on_interaction = 'demo'
	  }
    }
  }
}
