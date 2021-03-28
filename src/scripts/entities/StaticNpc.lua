StaticNpc = tdengine.entity('StaticNpc')
function StaticNpc:init(params)
   tdengine.register_collider(self.id)
end

function StaticNpc:update(dt)

end
