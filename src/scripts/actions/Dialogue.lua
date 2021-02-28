local Dialogue = tdengine.action('Dialogue')
local GLFW = require('glfw')

function Dialogue:init(params)
   self.which = params.dialogue
   self.data = tdengine.load_dialogue(self.which)
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
	  message = message .. 'Dialogue was: ' .. self.which
	  print(message)
	  
	  self.done = true
	  return
   end

   

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
   local value = tdengine.state[node.variable]
   if value == nil then
	  local message = 'Dialogue:process_set(): tried to set a variable, but it did not exist in the state. '
	  message = message .. 'Variable was: ' .. node.variable
	  tdengine.log(message, tdengine.log_flags.default)
	  return
   end

   tdengine.state[node.variable] = node.value
end

function Dialogue:process_text(node)
   self.text_box:begin(node.text)
end

function Dialogue:advance_until_input_needed()
   local is_input_needed = false
   while not is_input_needed do
	  if not self.current then
		 self.current = self:find_entry_node()
		 self:process(self.current)
		 
		 is_input_needed = self.current.kind == 'Text'
	  elseif #self.current.children == 0 then
		 local text_box = tdengine.find_entity('TextBox')
		 tdengine.destroy_entity(text_box:get_id())
		 
		 self.done = true
		 is_input_needed = true -- hack
	  elseif #self.current.children == 1 then
		 self.current = self.data[self.current.children[1]]
		 self:process(self.current)
		 
		 is_input_needed = self.current.kind == 'Text'

	  elseif #self.current.children > 1 then
		 self.choosing = true
		 self.choice = 1
		 
		 local children = {}
		 local choices = {}
		 for index, id in pairs(self.current.children) do
			table.insert(children, self.data[id])
			table.insert(choices, self.data[id].text)
		 end
		 self.current = children

		 self.text_box:choose(choices)
		 
		 is_input_needed = true
	  end
   end
end

function Dialogue:update(dt)
  self.text_box = tdengine.find_entity('TextBox')
  if not self.text_box then
	tdengine.create_entity('TextBox', {})
	self.text_box = tdengine.find_entity('TextBox')
  end

   if not self.text_box.active then
	  self:advance_until_input_needed()
   end

   if self.choosing then
	  if tdengine.was_pressed(GLFW.Keys.DOWN, tdengine.InputChannel.Game) then
		 self.choice = math.min(self.choice + 1, #self.current)
	  end
	  if tdengine.was_pressed(GLFW.Keys.UP, tdengine.InputChannel.Game) then
		 self.choice = math.max(self.choice - 1, 1)
	  end
	  self.text_box:highlight_choice(self.choice)

	  if tdengine.was_pressed(GLFW.Keys.SPACE, tdengine.InputChannel.Game) then
		 self.current = self.current[self.choice]
		 self.text_box:highlight_choice(-1)
		 self.choosing = false
		 self:advance_until_input_needed()
	  end
   else
	  local waiting = self.text_box.waiting
	  local done_with_node = self.text_box.done
	  
	  if tdengine.was_pressed(GLFW.Keys.SPACE, tdengine.InputChannel.Game) then
		 if not self.text_box.done then
			self.text_box:skip()
		 else
			self:advance_until_input_needed()
		 end
	  end
   end
end

tdengine.actions.Dialogue = Dialogue
