return {
  {
	name = 'Delay',
	time = .5
  },
  {
	 name = 'CreateEntity',
	 entity = 'StaticNpc',
	 data = {
		tag = 'oliver',
		params = {},
		components = {
		   Animation = {
			  current = 'oliver_stand'
		   },
		   Position = {
			  x = 0,
			  y = 0
		   }
		}
	 }
  },
  {
	 name = 'Teleport',
	 tag = 'oliver',
	 teleport_near_player = true,
	 offset = {
		x = 0,
		y = -.1
	 }
  },
  {
	name = 'Delay',
	time = .5
  },
  {
	 name = 'PanCamera',
	 marker = 'library:introduce:librarian'
  },
  {
	name = 'Delay',
	time = 1
  },
  {
	name = 'Dialogue',
	dialogue = 'intro_librarian_001'
  },
  {
	 name = 'Walk',
	 entity = 'Player',
	 markers = {
		'library:intro:waypoint:001',
		'library:intro:waypoint:002',
	 }
  },
  {
	 name = 'DisappearIntoPlayer',
	 tag = 'oliver'
  }
}
