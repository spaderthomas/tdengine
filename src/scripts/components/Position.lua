Position = tdengine.component('Position')
function Position:load(data)
   self.world = data.world or { x = 0, y = 0 }
end

function Position:init()
end

function Position:update()
end
