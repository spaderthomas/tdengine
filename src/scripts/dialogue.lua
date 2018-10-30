npc.intro_police.dialogue = {
   intro = {
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
}

npc.wilson.dialogue = {
   intro = {
	  text = "AAAAAAAAAAAAAAAHHHHHHHHHH!",
	  terminal = false;
	  responses = {
		 "press 1",
		 "press 2"
	  },
	  children = {
		 {
			text = "you pressed 1!",
			terminal = false;
			responses = {
			   "press 1",
			   "press 2"
			},
			children = {
			   {
				  text = "you pressed 1 AGAIN! WOW!!",
				  terminal = true;
				  responses = {},
				  children = {}
			   }
			},
		 },
		 {
			text = "you pressed 2!",
			terminal = true;
			responses = {},
			children = {},
		 },
	  },
   }
}

npc.intro1.dialogue = {
   intro = {
	  text = "...I heard it's part of the new crackdown...",
	  terminal = false,
	  responses = {},
	  children = {}
   }
}

npc.intro2.dialogue = {
   intro = {
	  text = "All of this racket over neural implants? I don't understand what the big deal is.",
	  terminal = false,
	  responses = {
		 "Neural implants?",
	  },
	  children = {
		 {
			text = "Don't tell me you don't know. After they put the ban on implants out yesterday, well, I mean, it wasn't really just a ban after all.",
			terminal = false,
			responses = {
			   "How was it more than just a ban?",
			},
			children = {
			   {
				  text = "Well, if you ban the kids from smoking cigarettes, usually that doesn't mean sending out a death squad to catch underaged smokers...I can't watch this",
				  terminal = false,
				  responses = {},
				  children = {}
			   }
			}
		 }
	  }
   }
   
}

