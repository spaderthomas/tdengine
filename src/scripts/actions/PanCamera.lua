local PanCamera = tdengine.action('PanCamera')

function PanCamera:init(params)
  if params.pan_to_player then
	local player = tdengine.find_entity('Player')
	local position = player:get_component('Position')
	self.target = tdengine.vec2(position.world)
  else
	local marker = tdengine.markers[params.marker]
	
	if marker.scene ~= tdengine.loaded_scene.name then
	  tdengine.log('@pan_camera_wrong_scene: ' .. marker.scene)
	  self.done = true
	  return
	end

	self.target = tdengine.vec2(marker.x, marker.y)
  end

  -- Here, the target is in world coordinates. But the camera is really an offset (so it is negative),
  -- and whatever the camera is at is at the bottom left of the screen. So adjust for those things.
  self.target = self.target:subtract(tdengine.vec2(.5, .5)):scale(-1)
  
  local start = tdengine.vec2(tdengine.camera())
  local initial_difference = self.target:subtract(start)
  self.per_frame = initial_difference:scale(.01)

j  tdengine.follow_player(false)
end

function PanCamera:update(dt)
  local camera = tdengine.vec2(tdengine.camera())
  if camera:equals(self.target, .001) then
	self.done = true
	return
  end
  
  tdengine.move_camera(self.per_frame.x, self.per_frame.y)
end

tdengine.actions.PanCamera = PanCamera
