return {
  {
	name = 'CreateEntity',
	entity = 'Oliver',
	data = {
	  components = {
		Position = {
		  marker = 'hub:introduce:oliver'
		}
	  }
	}
  },
  {
	name = 'Dialogue',
	dialogue = 'introduce_hub_001'
  },
  {
	name = 'Delay',
	time = 1
  },
  {
	name = 'Dialogue',
	dialogue = 'introduce_hub_002'
  },
  {
	name = 'Dialogue',
	dialogue = 'introduce_hub_003'
  },
  {
	name = 'DisappearIntoPlayer',
	entity = 'Oliver'
  },
}
