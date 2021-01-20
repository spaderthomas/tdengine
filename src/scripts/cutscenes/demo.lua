return {
   {
	  name = 'Sound',
	  sound = 'intro_theme_001',
	  loop = true
   },
   {
	  name = 'Delay',
	  time = 1000
   },
   {
	  name = 'FadeIn',
	  time = 3000
   },
   {
	  name = 'Delay',
	  time = 3000
   },
   {
	  name = 'Compound',
	  actions = {
		 {
			name = 'Animation',
			entity = 'Boon',
			animation = 'intro_surprise',
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
	  dialogue = 'intro_001'
   },
   {
	  name = 'MyCustomBullshit',
	  some_param = 'horselover fat',
	  somethining_else = 'evangelion unit-01'
   }
}
