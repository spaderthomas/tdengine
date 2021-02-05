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

   local process_until_text = function() end
   local next_node = function()
	  local num_children = #self.current.children
	  
	  -- We're done with the whole tree.
	  if num_children == 0 then
		 tdengine.text_box.resume() -- This will hide it and mark it inactive
		 
		 local text_box = tdengine.find_entity('TextBox')
		 tdengine.destroy_entity(text_box:get_id())
		 
		 self.done = true
		 
		 return
		 -- Not done with the whole tree, but can just do next node serially
	  elseif num_children == 1 then
		 self.current = self.data[self.current.children[1]]
		 
		 -- Right now, just perform all Set nodes until we have plain text
		 process_until_text()
		 
		 -- There is a next node. Display it.
		 if self.current then
			tdengine.text_box.begin(self.current.text)
		 end
		 -- Choice, branching, etc
	  else
		 self.choosing = true
		 self.choice = 1
		 
		 tdengine.text_box.clear()
		 
		 local children = {}
		 for index, id in pairs(self.current.children) do
			table.insert(children, self.data[id])
			tdengine.text_box.add_choice(self.data[id].text)
		 end
		 
		 self.current = children
		 
		 tdengine.text_box.begin('')
	  end
   end
   
   -- The case where we're giving the text box text for the first time
   if not tdengine.text_box.is_active() then
	  tdengine.create_entity('TextBox')
	  self.text_box = tdengine.find_entity('TextBox')
	  tdengine.text_box.begin(self.current.text)
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

   local waiting = tdengine.text_box.is_waiting()
   local done_with_node = tdengine.text_box.is_done()

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
		 next_node()
	  end
   end

end

tdengine.actions.Dialogue = Dialogue
