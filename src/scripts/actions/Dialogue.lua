local Dialogue = tdengine.action('Dialogue')
local GLFW = require('glfw')

function Dialogue:init(params)
   self.name = params.dialogue
   self.data = tdengine.load_dialogue(self.name)
   self.waiting = false
   self.choosing = false
   self.choice = nil

   self.current = nil
   for id, node in pairs(self.data) do
	  local is_entry_point = node.is_entry_point or false
	  if is_entry_point then
		 self.current = node
	  end
   end

   -- Error checking
   local message = nil
   if not self.current then
	  message = 'actions/Dialogue:init() :: no entry point node found. '
   end
   if self.current and self.current.kind == 'Choice' then
	  message = 'Choice nodes cannot be entry points'
   end
   
   if message then
	  message = message .. 'Dialogue was: ' .. self.name
	  print(message)
	  
	  self.done = true
	  return
   end

   tdengine.create_entity('TextBox')
   self.text_box = tdengine.find_entity('TextBox')
   
   -- tdengine.text_box.use_avatar(self.current.who)
   -- tdengine.text_box.use_voice(self.current.voice)
end

function Dialogue:process_set(node)
   self.text_box:begin('i set ' .. node.variable .. 'to ' .. tostring(node.value))
end

function Dialogue:process_text(node)
   self.text_box:begin(node.text)
end

function Dialogue:process_choices(node)
   self.choosing = true
   self.choice = 1
   
   for index, node in pairs(self.current) do
	  print(inspect(node))
   end
end

function finish()
   local text_box = tdengine.find_entity('TextBox')
   tdengine.destroy_entity(text_box:get_id())
   
   self.done = true
end

function Dialogue:advance_until_input_needed()
   while true do
	  if #self.current.children == 0 then
		 self:finish()
		 break
	  elseif #self.current.children == 1 then
		 self.current = self.data[self.current.children[1]]

		 if self.current.kind == 'Text' then
			self:process_text(self.current)
			break
		 end
		 
		 if self.current.kind == 'Set' then
			self:process_set(self.current)
		 end
	  elseif #self.current.children > 1 then
		 self:process_choices()

		 local children = {}
		 for index, id in pairs(self.current.children) do
			table.insert(children, self.data[id])
		 end
		 self.current = children
		 break
	  end
   end
end

function Dialogue:update(dt)
   -- The case where we're giving the text box text for the first time
   if not self.text_box.active then
	  self:advance_until_input_needed()
   end

   -- must be before next block, because next block sets choosing but when we
   -- set it we only wanna process it on the next frame
   if self.choosing then
	  if tdengine.was_pressed(GLFW.Keys.DOWN, tdengine.InputChannel.Game) then
		 self.choice = math.min(self.choice + 1, #self.current)
	  end
	  if tdengine.was_pressed(GLFW.Keys.UP, tdengine.InputChannel.Game) then
		 self.choice = math.max(self.choice - 1, 1)
	  end
	  tdengine.text_box.highlight_line(self.choice - 1)

	  if tdengine.was_pressed(GLFW.Keys.ENTER, tdengine.InputChannel.Game) then
		 self.current = self.current[self.choice]
		 tdengine.text_box.highlight_line(-1)
		 self.choosing = false
		 next_node()
	  end
   end

   local waiting = self.text_box.waiting
   local done_with_node = self.text_box.done

   if tdengine.was_pressed(GLFW.Keys.ENTER, tdengine.InputChannel.Game) then
	  -- If it's in the middle of some text, skip to the end
	  if not waiting then
		 tdengine.text_box.skip()
	  end
	  -- The text box is full, but the text we submitted isn't done yet
	  if waiting and not done_with_node then
		 tdengine.text_box.resume()
	  end
	  -- All text we submitted in begin() has been displayed. Next node.
	  if waiting and done_with_node then
		 self:advance_until_input_needed()
	  end
   end

end

tdengine.actions.Dialogue = Dialogue
