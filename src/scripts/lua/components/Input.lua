Input = tdengine.component('Input')
function Input:init()
  self.mask = tdengine.InputMask.Game
end

function Input:update()
end

function Input:was_key_pressed(key)
  return tdengine.was_key_pressed(key, self.mask)
end

function Input:is_key_down(key)
  return tdengine.is_key_down(key, self.mask)
end
