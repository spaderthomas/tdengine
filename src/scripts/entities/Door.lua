Door = tdengine.entity('Door')

function Door:init(params)
  self.fade_time = .5
   
  self.doing_transition = false
  self.time_remaining = 0
  self.marker = params.marker
  if not self.marker then
	 print('@no_marker_for_door: ' .. tostring(self.id))
	 params.marker = 'demo'
  end
  
  tdengine.register_collider(self.id)

  local aabb = self:get_component('BoundingBox')
  aabb.color = tdengine.colors.green
end

function Door:update(dt)
end

-- You can't change the scene in an update function because you will deallocate yourself.
-- You need to defer creating / destroying entities anyway...
function Door:update(dt)
  return
  if not self.doing_transition then return end
  
  self.time_remaining = self.time_remaining - dt
  if double_eq(self.time_remaining, 0, .01) then
	self.doing_transition = false
    local player = tdengine.find_entity('Player')
    tdengine.go_to_marker(player.id, self.marker)
	tdengine.snap_to_player()
	tdengine.enable_input_channel(tdengine.InputChannel.Player)
  end
end

function Door:on_collision(other)
  self.time_remaining = self.fade_time / 2
  self.doing_transition = true
  tdengine.fade_screen(self.fade_time)
  tdengine.go_to_marker(player.id, self.marker)
  tdengine.snap_to_player()  
end

function Door:pre_save()
  local aabb = self:get_component('BoundingBox')
  aabb:should_save(true)
end

function Door:save()
  return {
	 marker = self.marker
  }
end

