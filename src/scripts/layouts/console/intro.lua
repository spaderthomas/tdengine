local text_box = tdengine.find_entity('TextBox')
if text_box then
   text_box:reset()
end

layout('tiny')
scene('intro')
cutscene('demo')

