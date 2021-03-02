tdengine.state = {
   ['engine:disable_delay_actions'] = true,
   ['demo:001:heard_about_the_dead'] = false,
   ['demo:002:likes_psychedelic'] = true,
   ['demo:003:likes_americana'] = false,
   ['demo:004:likes_europe_72'] = false,
   ['demo:005:likes_evangelion'] = false,
   ['demo:006:likes_bebop'] = false,
   ['demo:007:likes_trigun'] = false,
   ['demo:008:likes_anime'] = false,
   ['demo:009:should_display_choice'] = false,
   ['jc:001:mentioned_librarian'] = false
}

tdengine.branch = {
   dead_branch = function()
	  if tdengine.state['demo:002:likes_psychedelic'] then
		 return 1
	  elseif tdengine.state['demo:003:likes_americana'] then
		 return 2
	  elseif tdengine.state['demo:004:likes_europe_72'] then
		 return 3
	  end

	  return -1
   end
}
