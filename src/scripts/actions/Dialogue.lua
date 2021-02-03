local Dialogue = tdengine.action('Dialogue')
local GLFW = require('glfw')

function Dialogue:init(params)
   print('Dialogue:init()')
   self.name = params.dialogue
   self.data = tdengine.load_dialogue(self.name)
   self.waiting = false

   self.current = nil
   for id, node in pairs(self.data) do
	  local is_entry_point = node.is_entry_point or false
	  if is_entry_point then
		 self.current = node
	  end
   end

   if not self.current then
	  local message = 'actions/Dialogue:init() :: no entry point node found. '
	  message = message .. 'Dialogue was: ' .. self.name
	  print(message)
	  
	  self.done = true
	  return
   end


   -- tdengine.text_box.use_avatar(self.current.who)
   -- tdengine.text_box.use_voice(self.current.voice)

end

function Dialogue:update(dt)
   if tdengine.text_box.is_done() or not tdengine.text_box.is_active() then
	  print('begin')
	  tdengine.text_box.begin(self.current.text)
   end
   
   if tdengine.was_pressed(GLFW.Keys.ENTER) then
	  --if tdengine.text_box.is_waiting() then
		 --tdengine.text_box.resume()
	  --else
		 --tdengine.text_box.skip()
	  --end
   end

   --if tdengine.text_box.is_waiting() then
	  -- advance to next node
	  --tdengine.text_box.add_choice()
   --end
   --self.done = true
end

tdengine.actions.Dialogue = Dialogue
