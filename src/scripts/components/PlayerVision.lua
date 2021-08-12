PlayerVision = tdengine.component('PlayerVision')
function PlayerVision:init(params)
  self.extents = params.extents or { x = .1, y = .1 }
  self.offset = params.offset or { x = 0, y = 0 }
end

function PlayerVision:save()
  return {
	extents = self.extents,
	offset = self.offset,
  }
end 

function PlayerVision:update()
  if not tdengine.state['engine:show_aabb'] then return end
  local position = self.parent:get_component('Position')
  local rect = {
	origin = tdengine.vec2(position.world):add(self.offset),
	extents = self.extents
  }
  tdengine.draw.rect_outline_world(rect, tdengine.colors.idk)
end
