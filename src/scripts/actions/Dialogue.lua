local Dialogue = tdengine.action('Dialogue')
local GLFW = require('glfw')

function Dialogue:init(params)
   self.name = params.dialogue
   self.data = tdengine.load_dialogue(self.name)
   self.waiting = false
   self.choosing = false

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

   self.text_box = tdengine.find_entity('text_box')
   if not text_box then
	  tdengine.create_entity('TextBox')
	  self.text_box = tdengine.find_entity('text_box')
   end

   
   -- tdengine.text_box.use_avatar(self.current.who)
   -- tdengine.text_box.use_voice(self.current.voice)
end

function Dialogue:update(dt)
   -- if it's a text node, then we want to display it / skip if input
   -- if it's a set node, set the variable and go to the child
   -- choice node means you want 
   --if self.current.kind == 'Set' then
	  --print('setting ' .. self.current.variable)
	  --self.current = self.data[self.current.children[0]]
   --end
   
   local cleanup = function()
   end
   
   -- The case where we're giving the text box text for the first time
   if not tdengine.text_box.is_active() then
	  print('begin: ' .. self.current.text)
	  tdengine.text_box.begin(self.current.text)
   end

   local waiting = tdengine.text_box.is_waiting()
   local done_with_node = tdengine.text_box.is_done()

   if tdengine.was_pressed(GLFW.Keys.ENTER, tdengine.InputChannel.Game) then
	  if not waiting then
		 tdengine.text_box.skip()
	  end
	  if waiting and done_with_node then
		 print('updating current...' .. inspect(self.current))
		 self.current = self.data[self.current.children[1]]
		 if not self.current then
			tdengine.text_box.resume()
			
			local text_box = tdengine.find_entity('TextBox')
			tdengine.destroy_entity(text_box:get_id())
			
			self.done = true
			
			return
		 end
		 
		 tdengine.text_box.begin(self.current.text)
	  end
	  if waiting and not done_with_node then
		 tdengine.text_box.resume()
	  end
   end

   tdengine.do_once(function(a)
		 tdengine.text_box.resume()
   end)
   --if tdengine.text_box.is_done() or not tdengine.text_box.is_active() then
	  --print('begin')
   --end
   
   --if tdengine.was_pressed(GLFW.Keys.ENTER) then
	  --if tdengine.text_box.is_waiting() then
		 --tdengine.text_box.resume()
	  --else
		 --tdengine.text_box.skip()
	  --end
   --end

   --if tdengine.text_box.is_waiting() then
	  -- advance to next node
	  --tdengine.text_box.add_choice()
   --end

end

tdengine.actions.Dialogue = Dialogue
