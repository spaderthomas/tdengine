Movement = tdengine.component('Movement')
function Movement:init()
  local position = self.parent:get_component('Position')
  self.wish = table.shallow_copy(position.world)

  self.speed = .01
end

function Movement:update(dt)
  local position = self.parent:get_component('Position')
  if not (self.wish.x == position.world.x and self.wish.y == position.world.y) then
	tdengine.move_entity(self.wish.x, self.wish.y)
  end
end

function Movement:move(xunits, yunits)
  self.wish.x = self.wish.x + self.speed * xunits
  self.wish.y = self.wish.y + self.speed * yunits
end

function Movement:teleport(x, y)
  self.wish.x = x
  self.wish.y = y
end
