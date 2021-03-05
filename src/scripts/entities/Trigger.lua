Trigger = tdengine.entity('Trigger')

function Trigger:init(params)
  self.on_collision_proc = params.on_collision
  tdengine.register_collider(self.id)
end

function Trigger:update(dt)
end

function Trigger:pre_save()
  local aabb = self:get_component('BoundingBox')
  aabb:should_save(true)
end

function Trigger:save()
  return {
	on_collision = self.on_collision_proc
  }
end

function Trigger:on_collision(other)
   if other:get_name() == 'Player' then
	  local proc = tdengine.triggers[self.on_collision_proc]
	  if proc then proc() end
   end
end

tdengine.triggers = {
   ['demo:001:fuck_off_randy'] = function(other)
	  print('fuck off, randy!')
   end
}
