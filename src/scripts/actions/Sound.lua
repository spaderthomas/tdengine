local Sound = tdengine.action('Sound')

function Sound:init(params)
   self.loop = params.loop or false
end

function Sound:update(dt)
   self.done = true
end

tdengine.actions.Sound = Sound
