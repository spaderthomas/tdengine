Door = tdengine.entity('Door')

function Door:init(params)
   self.where = params.where or ''
   tdengine.register_collider(self:get_id())
end

function Door:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw()
end
