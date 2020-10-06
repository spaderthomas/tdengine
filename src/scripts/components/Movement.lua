Movement = tdengine.component('Movement')
function Movement:init()
  self.wish = { x = 0, y = 0 }
  self.speed = .01
end

function Movement:update(dt)
end

function Movement:move(xunits, yunits)
  self.wish.x = self.speed * xunits
  self.wish.y = self.speed * yunits
  tdengine.internal.move_entity(self.parent:get_id())
end