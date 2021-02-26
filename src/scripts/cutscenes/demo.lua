return {
  {
	name = 'Sound',
	sound = 'intro_theme_001',
	loop = true,
	block = false
  },
  {
	name = 'Delay',
	time = 1
  },
  {
	name = 'FadeIn',
	time = 3
  },
  {
	name = 'Delay',
	time = 1
  },
  {
	name = 'Animation',
	entity = 'Player',
	animation = 'boon_surprise',
	play_when_finished = 'boon_stand',
	wait_for_current_animation = true,
	loop = false,
  },
  {
	name = 'Delay',
	time = 1
  },
  {
	name = 'Sound',
	sound = 'surprise',
	block = false
  },
  {
	name = 'Dialogue',
	dialogue = 'test_001'
  }
}
