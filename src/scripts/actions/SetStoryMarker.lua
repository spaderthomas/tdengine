local SetStoryMarker = tdengine.action('SetStoryMarker')

function SetStoryMarker:init(params)
  self.marker = params.marker
end

function SetStoryMarker:update(dt)
  tdengine.state['main:story_marker'] = self.marker
  self.done = true
end

tdengine.actions.SetStoryMarker = SetStoryMarker
