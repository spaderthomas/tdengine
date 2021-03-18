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

function Dialogue:process_single(node)
   if not node then
	  print('Dialogue:process(): tried to process a nil node')
	  self.bad_branch = true
   elseif node.kind == 'Text' then
	  self.text_box:begin(node.text)
   elseif node.kind == 'Set' then
	  self:process_set(node)
   elseif node.kind == 'Empty' then
	  return
   elseif node.kind == 'Branch' then
	  local branch_result = self:evaluate_branch(node)
	  assert(branch_result.kind == 'Text', '@branch_did_not_evaluate_to_text')
	  
	  self.current = branch_result
	  self:process_single(self.current)
   end
end

function Dialogue:process_set(node)
   local value = tdengine.state[node.variable]
   if value == nil then
	  local message = 'dialogue:@variable_does_not_exist:'
	  message = message .. node.variable
	  tdengine.log(message)
	  return
   end
   
   tdengine.state[node.variable] = node.value
end

function Dialogue:evaluate_branch(node)
   -- Evaluate the state to figure out which child we're going to
   local child_id = -1
   local branch_on = tdengine.branches[node.branch_on]
   if branch_on then
	  child_index = branch_on()
   else
	  local state = tdengine.state[node.branch_on]
	  child_index = ternary(state, 1, 2)
   end

   -- Grab the child's data from our list
   local child_id = node.children[child_index]
   local child = self.data[child_id]

   -- A couple things could have happened here:
   -- 1. Your branch function return an invalid index
   -- 2. You didn't hook enough children to the branch node
   if not child then return child end

   -- A branch node doesn't have to evaluate immediately. But the only thing it 
   -- makes sense to do before you evaluate to a Text or Choice is to set variables.
   while child and child.kind == 'Set' do
	  self:process_set(child)
	  child = self.data[child.children[1]]
   end

   return child
end

function Dialogue:finish()
   local text_box = tdengine.find_entity('TextBox')
   tdengine.destroy_entity(text_box:get_id())
   
   self.done = true
end

function Dialogue:advance_until_input_needed()
   local is_input_needed = false
   while not is_input_needed do
	  if self.bad_branch then
		 print('bad branch')
		 self:finish()
		 is_input_needed = true -- hack

	  -- This is only true the first time you call this.
	  elseif not self.current then
		 self.current = self:find_entry_node()
		 self:process_single(self.current)
		 
		 is_input_needed = self.current.kind == 'Text'
      -- If the current node has no children, we're done!
	  elseif #self.current.children == 0 then
		 self:finish()
		 is_input_needed = true -- hack
	  -- If the current node has one child, just eat it. You only need to wait for
	  -- input if the next node is a Text node (because Text nodes don't advance
	  -- until the player hits the space bar)
	  elseif #self.current.children == 1 then
		 self.current = self.data[self.current.children[1]]
		 self:process_single(self.current)
		 
		 is_input_needed = self.current and self.current.kind == 'Text'
      -- > 1 child means you hit a choice node
	  elseif #self.current.children > 1 then
		 self.choosing = true
		 self.choice = 1
		 
		 local children = {}
		 local choices = {}
		 for index, id in pairs(self.current.children) do
			local child = self.data[id]
			if child.kind == 'Choice' then
			   table.insert(children, child)
			   table.insert(choices, self.data[id].text)
			elseif child.kind == 'Branch' then
			   local maybe_choice = self:evaluate_branch(child)
			   if maybe_choice then
				  assert(maybe_choice.kind == 'Choice', 'need choice node')
				  table.insert(children, maybe_choice)
				  table.insert(choices, maybe_choice.text)
			   end
			end
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
