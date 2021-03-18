local dead_branch = function()
  if tdengine.state['demo:002:likes_psychedelic'] then
	return 1
  elseif tdengine.state['demo:003:likes_americana'] then
	return 2
  elseif tdengine.state['demo:004:likes_europe_72'] then
	return 3
  end
  
  return -1
end

return {
  dead_branch = dead_branch
}
