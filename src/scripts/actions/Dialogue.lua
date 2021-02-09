local Dialogue = tdengine.action('Dialogue')
local GLFW = require('glfw')

function Dialogue:init(params)
   self.name = params.dialogue
   self.data = tdengine.load_dialogue(self.name)
   self.waiting = false
   self.choosing = false
   self.choice = nil
   self.current = nil

   -- Error checking
   local message = nil
   local entry = self:find_entry_node()
   if not entry then
	  message = 'actions/Dialogue:init() :: no entry point node found. '
   end
   if entry and entry.kind == 'Choice' then
	  message = 'Choice nodes cannot be entry points'
   end
   
   if message then
	  message = message .. 'Dialogue was: ' .. self.name
	  print(message)
	  
	  self.done = true
	  return
   end

   
   self.text_box = tdengine.find_entity('TextBox')
   if not self.text_box then
	  tdengine.create_entity('TextBox')
	  self.text_box = tdengine.find_entity('TextBox')
   end

   self:foo()
   -- tdengine.text_box.use_avatar(self.current.who)
   -- tdengine.text_box.use_voice(self.current.voice)
end

function Dialogue:find_entry_node()
   for id, node in pairs(self.data) do
	  local is_entry_point = node.is_entry_point or false
	  if is_entry_point then
		 return node
	  end
   end

   return nil
end

function Dialogue:process(node)
   if node.kind == 'Text' then
	  self:process_text(node)
   elseif node.kind == 'Set' then
	  self:process_set(node)
   end
end

function Dialogue:process_set(node)
   self.text_box:begin('i set ' .. node.variable .. 'to ' .. tostring(node.value))
end

function Dialogue:process_text(node)
   self.text_box:begin(node.text)
end

function Dialogue:foo()
   print('advance_until_input_needed(): begin')
   local is_input_needed = false
   while not is_input_needed do
	  print('advance_until_input_needed(): loop: (' .. tostring(self.done) .. ', ' .. tostring(is_input_needed) .. ')')
	  if not self.current then
		 print('advance_until_input_needed(): processing first node')
		 self.current = self:find_entry_node()
		 self:process(self.current)
		 
		 is_input_needed = self.current.kind == 'Text'
	  elseif #self.current.children == 0 then
		 print('advance_until_input_needed(): done!')
		 local text_box = tdengine.find_entity('TextBox')
		 tdengine.destroy_entity(text_box:get_id())
		 
		 self.done = true
		 is_input_needed = true -- hack
	  elseif #self.current.children == 1 then
		 print('advance_until_input_needed(): processed one')
		 self.current = self.data[self.current.children[1]]
		 self.process(self.current)
		 
		 is_input_needed = self.current.kind == 'Text'

	  elseif #self.current.children > 1 then
		 print('advance_until_input_needed(): choice!')

		 self.choosing = true
		 self.choice = 1
		 
		 local children = {}
		 for index, id in pairs(self.current.children) do
			table.insert(children, self.data[id])
		 end
		 self.current = children
		 
		 is_input_needed = true
	  end

	  print('advance_until_input_needed(): current node is ' .. inspect(self.current))
	  print('advance_until_input_needed(): input needed: ' .. tostring(is_input_needed))
   end

   print('advance_until_input_needed():  input is needed!')
end

function Dialogue:update(dt)
   -- must be before next block, because next block sets choosing but when we
   -- set it we only wanna process it on the next frame
   --[[
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
   --]]
   
   local waiting = self.text_box.waiting
   local done_with_node = self.text_box.done

   if tdengine.was_pressed(GLFW.Keys.ENTER, tdengine.InputChannel.Game) then
	  -- If it's in the middle of some text, skip to the end
	  if not waiting then
		 self.text_box:skip()
	  end
	  -- The text box is full, but the text we submitted isn't done yet
	  if waiting and not done_with_node then
		 tdengine.text_box.resume()
	  end
	  -- All text we submitted in begin() has been displayed. Next node.
	  if waiting and done_with_node then
		 print('Dialogue:update() :: moving to next node')
		 self.current = self.data[self.current.children[1]]
		 self.text_box:begin(node.text)
		 --self:foo()
	  end
   end

   --[[
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
   --]]

end

tdengine.actions.Dialogue = Dialogue
