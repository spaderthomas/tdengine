HubManager = tdengine.entity('HubManager')
function HubManager:init(params)
end

function HubManager:setup()
  tdengine.apply_overlay()

  -- Run whatever bullshit custom setup code you want
  local story_marker = tdengine.state['main:story_marker']
  if story_marker == tdengine.story_markers.LEFT_JANITOR_CLOSET then
	print('custom setup code')
  end
end

function HubManager:cleanup()
end

function HubManager:update(dt)
end
