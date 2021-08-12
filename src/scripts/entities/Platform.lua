Platform = tdengine.entity('Platform')
function Platform:init(params)
  tdengine.register_collider(self.id)
end

function Platform:update(dt)
end
