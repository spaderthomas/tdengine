local text_box = tdengine.find_entity('TextBox')
if not text_box then
   tdengine.create_entity('TextBox')
   text_box = tdengine.find_entity('TextBox')
end

local inspect = require('inspect')
print(inspect(text_box))

