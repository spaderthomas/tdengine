return {
   manager = 'Battle',
   entities = {
	{
	  name = 'Battle',
	  params = {},
	  components = {}
	},
    {
      components = {
        Animation = {
          current = "bg_battle_grass"
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
	  name = 'Platform',
	  tag = 'player_platform',
	  params = {
		platform = 'platform_grass_player'
	  },
	  components = {
		Position = {
		  world = { x = -.25, y = .3 }
		}
	  }
	},
	{
	  name = 'Platform',
	  tag = 'opponent_platform',
	  params = {
		platform = 'platform_grass_opponent'
	  },
	  components = {
		Position = {
		  world = { x = 1.3, y = .6 }
		}
	  }
	},
   }
}
