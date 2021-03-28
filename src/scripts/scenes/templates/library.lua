return {
   manager = 'Library',
   entities = {
	{
	   name = 'Library',
	   params = {},
	   components = {}
	},
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
		marker = 'library_entrance'
      }
    }
  }
}
