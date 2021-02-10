return {
   {
	  name = 'Sound',
	  sound = 'intro_theme_001',
	  loop = true,
	  block = false
   },
   {
	  name = 'Delay',
	  time = 0
   },
   {
	  name = 'FadeIn',
	  time = 3
   },
   {
	  name = 'Delay',
	  time = 0
   },
   {
	  name = 'Compound',
	  actions = {
		 {
			name = 'Animation',
			entity = 'Player',
			animation = 'boon_surprise',
			loop = false,
		 },
		 {
			name = 'Animation',
			entity = 'Oliver',
			animation = 'oliver_walkdown',
			loop = false,
			block = false
		 },
		 {
			name = 'Sound',
			sound = 'surprise',
			block = false
		 }
	  }
   },
   {
	  name = 'Dialogue',
	  dialogue = 'choice'
   },
   {
	  name = 'MyCustomBullshit',
	  some_param = 'horselover fat',
	  somethining_else = 'evangelion unit-01'
   }
}
