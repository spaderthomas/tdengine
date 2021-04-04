Door = tdengine.entity('Door')

function Door:init(params)
  self.doing_transition = false
  self.time_until_change = 0
  self.time_until_unlock_input = 0
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
function Door:fucked(dt)
  if not self.doing_transition then return end
  
  self.time_until_change = self.time_until_change - dt
  self.time_until_unlock_input = self.time_until_unlock_input - dt
  if double_eq(self.time_until_change, 0, .01) then
	tdengine.change_scene(self.marker.scene)

	local player = tdengine.find_entity('Player')
	tdengine.teleport_entity(player.id, self.marker.x, self.marker.y)
	tdengine.snap_to_player()
  end

  if double_eq(self.time_until_unlock_input, 0, .01) then
	tdengine.enable_input_channel(tdengine.InputChannel.Player)
	--tdengine.destroy_entity(self.id)
  end

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

function Door:on_collision(other)
  local player = tdengine.find_entity('Player')
  tdengine.fade_screen(.5)
  tdengine.go_to_marker(player.id, self.marker)
  tdengine.snap_to_player()  
end

  function Door:fucked(other)
	if other:get_name() == 'Player' then
	  --self:persist()
	  self.doing_transition = true
	  
	  local fade_time = .5
	  self.time_until_change = fade_time
	  self.time_until_unlock_input = fade_time * 2
	  tdengine.fade_screen(fade_time)
	end
	--tdengine.disable_input_channel(tdengine.InputChannel.Player)
  end
