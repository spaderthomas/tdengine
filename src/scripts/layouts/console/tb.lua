local text_box = tdengine.find_entity('TextBox')
if not text_box then
   tdengine.create_entity('TextBox')
   text_box = tdengine.find_entity('TextBox')
end
local text = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum feugiat, turpis non mollis malesuada, nunc lorem elementum erat, eu faucibus diam ante eu velit. Nulla facilisi. Nulla non purus et nibh congue dapibus a eu dolor. Fusce mattis mauris vitae libero consectetur feugiat. Donec ac leo vel mauris tempus suscipit ut quis ipsum. Fusce convallis lectus at lorem consequat faucibus. Sed ac justo ligula.'
print('c++')
tdengine.text_box.begin(text)
print('lua')
text_box:begin(text)

