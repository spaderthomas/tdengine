local Sound = tdengine.action('Sound')

function Sound:init(params)
   print('sound:init()')
   print('sound is: ' .. params.sound)
   self.wait = params.wait or false
end

function Sound:update(dt)
   print('sound:update()')
end

tdengine.actions.Sound = Sound
