-- Script
entity.wilson.scripts.intro = {
   {
	  kind = "And_Action",
	  actions = {
		 {
			kind = "Wait_For_Interaction_Action",
			is_blocking = false
		 },
		 {
			kind = "Movement_Action",
			is_blocking = false,
			dest = {
			   x = .4,
			   y = .4
			}
		 }
	  }
   },
   {
	  kind = "Dialogue_Action",
	  dialogue = {
		 text = "*intro dialogue*",
		 terminal = true,
		 responses = {
			"dialogue makes games fun lol"
		 },
		 children = {}
	  }
   }
}


-- State machine
entity.wilson.State_Machine.intro = {
   task = entity.wilson.scripts.intro,
   transitions = {
	  {
		 vars = {
			a = true,
		 },
		 next_state = "main1"
	  }
   }
}
