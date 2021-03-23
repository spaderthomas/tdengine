return {
  manager = 'HubManager',
  entities = {
	{
	  name = 'HubManager',
	  params = {},
	  components = {}
	},
    {
      components = {
        Animation = {
          current = "bg_hub"
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
            x = 0.12089204788208008,
            y = 0.095306843519210815
          },
          offset = {
            x = 0,
            y = 0
          }
        },
        Position = {
          world = {
            x = 3.6605854034423828,
            y = 3.2043693065643311
          }
        }
      },
      name = "Door",
      params = {
        marker = "library"
      }
    },
    {
      components = {
        BoundingBox = {
          extents = {
            x = 0.028560250997543335,
            y = 0.23465701937675476
          },
          offset = {
            x = 0,
            y = 0
          }
        },
        Position = {
          world = {
            x = 2.1952559947967529,
            y = 2.3697376251220703
          }
        }
      },
      name = "Door",
      params = {
        marker = "janitor_closet"
      }
    }
  }
}
