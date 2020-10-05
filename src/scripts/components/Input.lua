Input = tdengine.component('Input')
function Input:init()
  self.channel = tdengine.InputChannel.Editor
end

function Input:update()
end

function Input:was_pressed(key)
  return tdengine.was_pressed(key, self.channel)
end

function Input:is_down(key)
  return tdengine.is_down(key, self.channel)
end

function Input:set_channel(channel)
  self.channel = channel
end

function Input:enable()
  tdengine.enable_input_channel(self.channel)
end

function Input:disable()
  tdengine.disable_input_channel(self.channel)
end
