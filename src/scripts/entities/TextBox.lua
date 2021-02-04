TextBox = tdengine.entity('TextBox')
function TextBox:init()
   tdengine.register_collider(self:get_id())
end

function TextBox:update(dt)
end
