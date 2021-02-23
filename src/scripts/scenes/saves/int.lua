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
            x = 0.14625518023967743,
            y = 0.45921644568443298
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
        Animation = {
          animations = {
            box16 = {
              "box16.png"
            }
          },
          current = "box16"
        },
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
            x = 0.25026038289070129,
            y = 0.2906481921672821
          }
        }
      },
      name = "Interactable",
      params = {
        on_interaction = "demo"
      }
    }
  }
}