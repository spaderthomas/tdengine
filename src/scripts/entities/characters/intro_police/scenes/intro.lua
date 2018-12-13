entity.intro_police.dialogue.intro = {
   text = "ID?",
   terminal = false,
   responses = {
	  "<Give him your identification>",
	  "Leave me alone!"
   },
   children = {
	  {
		 text = "You're a citizen. Why did you ignore the message beamed to your chip?",
		 terminal = false,
		 responses = {
			"What message?"
		 },
		 children = {
			{
			   text = "I don't have time to listen to you play dumb. Check your chip and do as it says. I've got my eye on you.",
			   terminal = true,
			   responses = {},
			   children = {}
			}
		 }
	  },
	  {
		 text = "Uh...you'll have to come to the station sir!",
		 terminal = true,
		 responses = {},
		 children = {}
	  }
   }
}
