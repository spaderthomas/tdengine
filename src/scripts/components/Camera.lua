Camera = tdengine.component('Camera')
function Camera:init()
  self.offset = {
	x = 0,
	y = 0
  }
  self.following = nil
  self.panning = false
end

function Camera:update()
  if (not panning and following) then
	local position = self.following:get_component('Position')
	self.offset.x = position.x - .5
	self.offset.y = position.y - .5
	return
  end

  tdengine.set_camera_offset(self.offset.x, self.offset.y)
end
