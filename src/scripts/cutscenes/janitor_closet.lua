return {
  {
	name = 'Sound',
	sound = 'intro_theme_001',
	loop = true,
	block = false
  },
  {
	name = 'Delay',
	time = 3
  },
  {
	name = 'Dialogue',
	dialogue = 'jc_prod'
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
	name = 'Dialogue',
	dialogue = 'jc_prod'
  },
  {
	name = 'DisappearIntoPlayer',
	entity = 'Oliver'
  },
}
