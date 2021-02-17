Door = tdengine.entity('Door')

function Door:init(params)
   self.where = params.where or ''
   tdengine.register_collider(self:get_id())
end

function Door:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw()
end

function Door:on_collision(other)
   if other:get_name() == 'Player' then
	  print('plyr')
   end
end
