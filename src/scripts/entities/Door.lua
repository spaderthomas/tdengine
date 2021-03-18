Door = tdengine.entity('Door')

function Door:init(params)
  self.where = params.where or ''

  if type(params.position) == 'string' then
	 self.shortcut = params.position
	 self.position = tdengine.locations[params.position]
  else
	 self.position = params.position
  end

  if not self.position then
	 print('@no_position_for_door: ' .. tostring(self.id))
	 self.position = self.position or tdengine.vec2(0, 0)
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
	 where = self.where,
	 position = self.shortcut or self.position
  }
end

function Door:on_collision(other)
  if other:get_name() == 'Player' then
	tdengine.save_current_scene_to_memory()

	local loaders = {
	  tdengine.load_scene_from_memory,
	  tdengine.load_scene_from_template,
	  tdengine.load_scene_from_disk
	}
	
	for index, loader in pairs(loaders) do
	   if loader(self.where) then
		  tdengine.fade_screen(.5)
		  tdengine.teleport_entity(other.id, self.position.x, self.position.y)
		  tdengine.snap_to_player()
		  return
	   end
	end

  end
end
