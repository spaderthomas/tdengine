return {
  loaded_scene = {
    name = "library",
    path = "scenes/templates/library"
  },
  player = {
    position = {
      x = 2.3249995708465576,
      y = 2.5050086975097656
    }
  },
  scenes = {
    library = {
      entities = {
        {
          components = {
            Animation = {
              current = "bg_library"
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
            BoundingBox = {
              extents = {
                x = 0.11035564541816711,
                y = 0.10174417495727539
              },
              offset = {
                x = 0,
                y = 0
              }
            },
            Position = {
              world = {
                x = 2.3410143852233887,
                y = 1.9465311765670776
              }
            }
          },
          name = "Door",
          params = {
            position = "library",
            where = "library"
          }
        }
      }
    }
  },
  state = {
    ["demo:001:heard_about_the_dead"] = true,
    ["demo:002:likes_psychedelic"] = true,
    ["demo:003:likes_americana"] = false,
    ["demo:004:likes_europe_72"] = true,
    ["demo:005:likes_evangelion"] = false,
    ["demo:006:likes_bebop"] = true,
    ["demo:007:likes_trigun"] = false,
    ["demo:008:likes_anime"] = true,
    ["demo:009:should_display_choice"] = true,
    ["engine:disable_delay_actions"] = true,
    ["jc:001:mentioned_librarian"] = false
  }
}