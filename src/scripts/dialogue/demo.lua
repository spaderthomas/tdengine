local boon_s_001 = {
   text = 'AHH!',
   who = 'player'
}

local oliver_s_001 = {
   text = "Oh! You've woken up, sir! I was getting worried!",
   who = 'oliver'
}

local boon_s_002 = {
   text = 'Where am I?',
   who = 'player'
}

local oliver_s_002 = {
   text = "You really don't know? I've never found one down here who didn't know. You're in Limbo, sir!",
   who = 'oliver'
}

-- First questions
local boon_q_001 = {
   text = 'what is A?',
   who = 'player'
}
local boon_q_002 = {
   text = 'what is B?',
   who = 'player'
}

local oliver_a_001 = {
   text = "well, clearly it's A...",
   who = 'oliver'   
}
local oliver_a_002 = {
   text = "it's B, dude...",
   who = 'oliver'   
}

local graph = tdengine.make_dialogue_graph()
graph.add_node(boon_s_001)
graph.add_node(oliver_s_001)
graph.add_node(boon_q_001)
graph.add_node(boon_q_002)

graph.get_node('boon_s_001').add_child(oliver_s_001)

graph.get_node('boon_s_001').add_child(oliver_s_001)

return {
   boon_s_001,
   oliver_s_001,
   subtree = {
	  choices = {
		 boon_q_001,
		 boon_q_002
	  }
   }
}
