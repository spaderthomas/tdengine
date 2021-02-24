Door = tdengine.entity('Door')

function Door:init(params)
  self.where = params.where or ''
  local graphic = self:get_component('Graphic')
  graphic.draw = false
  
  tdengine.register_collider(self.id)

end

function Door:update(dt)
  local aabb = self:get_component('BoundingBox')
  aabb:draw()
end

function Door:pre_save()
  local aabb = self:get_component('BoundingBox')
  aabb:should_save(true)
end

function Door:save()
  return {
	where = self.where
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
	   end
	end
  end
end
