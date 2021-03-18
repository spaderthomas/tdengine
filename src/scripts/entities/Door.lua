Door = tdengine.entity('Door')

function Door:init(params)
  self.marker = params.marker

  if not self.marker then
	 print('@no_marker_for_door: ' .. tostring(self.id))
	 self.marker = 'demo'
  end
  
  tdengine.register_collider(self.id)
end

function Door:update(dt)
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
  if other:get_name() == 'Player' then
	tdengine.save_current_scene_to_memory()
	tdengine.fade_screen(.5)
	tdengine.go_to_marker(self.marker)
  end
end
