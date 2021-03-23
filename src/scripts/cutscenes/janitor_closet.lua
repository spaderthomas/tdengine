return {
  {
	name = 'CreateEntity',
	entity = 'Oliver',
	data = {
	  components = {
		Position = {
		  marker = 'oliver_jc_cutscene'
		}
	  }
	}
  },
  {
	name = 'Teleport',
	entity = 'Player',
	marker = 'janitor_closet:player'
  },
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
	dialogue = 'jc_introduce'
	--dialogue = 'branch_on_function'
  },
  {
	name = 'DisappearIntoPlayer',
	entity = 'Oliver'
  },
  {
	name = 'SetStoryMarker',
	marker = tdengine.story_markers.LEFT_JANITOR_CLOSET
  }
}
