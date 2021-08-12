Platform = tdengine.entity('Platform')
function Platform:init(params)
  tdengine.register_collider(self.id)
  local animation = self:get_component('Animation')
  animation:begin(params.platform)
end

function Platform:update(dt)
end
