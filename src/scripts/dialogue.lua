npc.intro_police.dialogue = {
   intro = {
	  text = "I know you haven't forgotten. Come with me, citizen.",
	  terminal = false,
	  responses = {
		 "What?"
	  },
	  children = {
		 {
			text = "Everyone got the message sent to their chip. All citizens must attend the public execution. You're not special.",
			terminal = false,
			responses = {
			   "But I -- of course I got the message. It merely slipped my mind. I'll be on my way."
			},
			children = {
			   {
				  text = "On your way then. Don't think I won't have my eye out for you.",
				  terminal = true,
				  responses = {},
				  children = {}
			   }
			}
		 }
	   
	  }
   },
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

