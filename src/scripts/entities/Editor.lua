local GLFW = require('glfw')
local inspect = require('inspect')

local EditState = {
  Idle = 'Idle',
  Selected = 'Selected',
  HoldingSelection = 'HoldingSelection', 
  RectangleSelect = 'RectangleSelect',
  ReadyToDrawGeometry = 'ReadyToDrawGeometry',
  DrawingGeometry = 'DrawingGeometry',
}

Editor = tdengine.entity('Editor')
function Editor:load(data)
end

function Editor:init()
  self:persist()
   
  self.options = {
	 show_bounding_boxes = false,
	 show_minkowksi = false,
	 show_imgui_demo = false,
	 show_framerate = false
  }
  
  self.state = EditState.Idle
  
  self.selected = nil
  self.position_when_selected = { x = 0, y = 0 }
  self.hovered = nil
  
  self.ded = {
	 editor = imgui.InputTextMultiline.new(),
	 nodes = {},
	 layout_data = {},
	 loaded = '',
	 selected = nil,
	 connecting = nil,
	 disconnecting = nil,
	 scrolling = tdengine.vec2(0, 0),
	 window_position = tdengine.vec2(0, 0)
  }
  
  -- Stored as screen coordinates, converted to world when we submit the geometry
  self.last_click = { x = 0, y = 0 }
  
  self.filter = imgui.TextFilter.new()
  self.id_filter = imgui.TextFilter.new()

  self.display_framerate = 0
  self.average_framerate = 0
  self.frame = 0

  self:do_not_save()
  
  local input = self:get_component('Input')
  input:set_channel(tdengine.InputChannel.Editor)
  input:enable()
end

function Editor:update(dt)
  local dbg = self:get_component('Debug')
  self:calculate_framerate()
  
  self:handle_input()
  
  imgui.SetNextWindowSize(300, 300)
  imgui.Begin("tded v2.0", true)
  imgui.Text('frame: ' .. tostring(self.frame))
  imgui.Text('fps: ' .. tostring(self.display_framerate))
  if imgui.Button("save imgui.ini") then
  	 tdengine.internal.save_imgui_layout()
  end

  imgui.Begin("scene", true)
  self:draw_entity_viewer()
  imgui.Separator()
  self:draw_selected_view()
  imgui.Separator()
  self:draw_tools()
  imgui.End()
  imgui.End()

  self:do_geometry()

  self:dialogue_editor()
  
  local input = self:get_component('Input')
  
  -- In Idle, we check for any clicks that would select an entity
  if self.state == EditState.Idle then
	 self:check_for_new_selection()
  -- In Selected, we also check for any clicks that would select an entity
  elseif self.state == EditState.Selected then
	 self:check_for_new_selection()
  -- If we're holding the selection, try to drag it
  elseif self.state == EditState.HoldingSelection then
	 if input:is_down(GLFW.Keys.MOUSE_BUTTON_1) then
		local diff = self:get_mouse_vector()
		local new_position = {
		   x = self.position_when_selected.x + diff.x,
		   y = self.position_when_selected.y + diff.y
		}
		tdengine.teleport_entity(self.selected:get_id(), new_position.x, new_position.y)
	 else
		self.state = EditState.Selected
		print('You let go of the left mouse button')
	 end
  end
end

function Editor:do_geometry()
   local input = self:get_component('Input')

   if self.state == EditState.ReadyToDrawGeometry then
  	 if input:was_pressed(GLFW.Keys.MOUSE_BUTTON_1) then
	 	self.last_click.x = tdengine.get_cursor_x()
	 	self.last_click.y = tdengine.get_cursor_y()
		self.state = EditState.DrawingGeometry
	 end
  elseif self.state == EditState.DrawingGeometry then
     if input:is_down(GLFW.Keys.MOUSE_BUTTON_1) then
	    local rect = self:get_mouse_rect()
	    tdengine.draw.rect_outline_screen(rect, tdengine.colors.red)
  	 else
		local box = self:create_entity('Box')
		local rect = self:get_mouse_rect()
		
		local aabb = box:get_component('BoundingBox')
		aabb.extents = rect.extents
		
		local position = tdengine.screen_to_world(rect.origin)
		tdengine.register_collider(box:get_id())
		tdengine.teleport_entity(box:get_id(), position.x, position.y)
		
		self.state = EditState.Idle
		self.last_click.x = 0
		self.last_click.y = 0
	 end
  end
end

function Editor:calculate_framerate()
   local framerate = tdengine.framerate or 0
   self.average_framerate = self.average_framerate * .5
   self.average_framerate = self.average_framerate + framerate * .5
   self.frame = self.frame + 1
   if self.frame % 20 == 0 then
	  self.display_framerate = self.average_framerate
   end   
end

function Editor:handle_input()
  self:adjust_camera()
  
  local input = self:get_component('Input')
  if input:was_pressed(GLFW.Keys.ESCAPE) then
	 local graphic = self.selected:get_component('Graphic')
	 if graphic ~= nil then
		graphic.flags = 0
	 end
	self.selected = nil
	
	self.state = EditState.Idle
  end

  input:set_channel(tdengine.InputChannel.All)
  if input:was_pressed(GLFW.Keys.LEFT_CONTROL) then
    tdengine.internal.toggle_console()
  end
  input:set_channel(tdengine.InputChannel.Editor)
end

function Editor:adjust_camera()
  local input = self:get_component('Input')

  local offset = { x = 0, y = 0 }
  if input:is_down(GLFW.Keys.W) then
	offset.y = offset.y - .02
  end
  
  if input:is_down(GLFW.Keys.A) then
	offset.x = offset.x + .02
  end
  
  if input:is_down(GLFW.Keys.S) then
	offset.y = offset.y + .02
  end
  
  if input:is_down(GLFW.Keys.D) then
	offset.x = offset.x - .02
  end

  tdengine.move_camera(offset.x, offset.y)
end

function Editor:check_for_new_selection()
   local input = self:get_component('Input')

   if input:was_pressed(GLFW.Keys.MOUSE_BUTTON_1) then
	  local cursor = tdengine.cursor()
	  cursor = tdengine.screen_to_world(cursor)

	  -- Deselect whatever is already selected, whether this click hit anything
	  -- or not
	  if self.selected ~= nil then
		 local graphic = self.selected:get_component('Graphic')
		 if graphic ~= nil then
			graphic.flags = 0
		 end
	  end

	  self.selected = tdengine.ray_cast(cursor.x, cursor.y)
	  if self.selected ~= nil then
		 self.state = EditState.HoldingSelection
		 self.last_click = tdengine.cursor()

		 -- Mark down where it was when we clicked it, so we can drag it
		 local position = self.selected:get_component('Position')
		 if position ~= nil then
			self.position_when_selected = {
			   x = position.world.x,
			   y = position.world.y
			}
		 end

		 -- Highlight it (should use bitwise ops to toggle this)
		 local graphic = self.selected:get_component('Graphic')
		 if graphic ~= nil then
			graphic.flags = 1
		 end
	  end
   end
end

function Editor:draw_tools()
  if imgui.Button("Add Geometry") then
  	 local input = self:get_component('Input')
	 self.state = EditState.ReadyToDrawGeometry
  end

  if imgui.Button("Save") then
	 tdengine.save_scene(tdengine.loaded_scene)	 
  end

end

function Editor:draw_entity_viewer()
  self.filter:Draw("Filter by name")
  for id, entity in pairs(tdengine.entities) do
	local name = entity:get_name()
	local id = tostring(entity:get_id())
	if self.filter:PassFilter(name) or self.filter:PassFilter(id) then
	  imgui.extensions.Entity(entity)
	end
  end
end

function Editor:draw_selected_view()
   if self.selected ~= nil then
	  if imgui.Button('Delete') then
		 tdengine.destroy_entity(self.selected:get_id())
		 self.selected = nil
	  end
	  
	  if self.selected ~= nil then
		 imgui.extensions.Entity(self.selected)
	  end
   end
end

function Editor:get_mouse_rect()
   return {
      extents = {
	     x = math.abs(tdengine.get_cursor_x() - self.last_click.x),
		 y = math.abs(tdengine.get_cursor_y() - self.last_click.y)
	  },
	  origin = {
	     x = (tdengine.get_cursor_x() + self.last_click.x) / 2,
	     y = (tdengine.get_cursor_y() + self.last_click.y) / 2
	  }
   }
end

function Editor:get_mouse_vector()
   return {
	  x = tdengine.get_cursor_x() - self.last_click.x,
	  y = tdengine.get_cursor_y() - self.last_click.y
   }
end

-- Take a coordinate in canvas' world space and convert it to
-- the window's screen space (for DrawList)
function Editor:canvas_world_to_window_screen(canvas_world)
   local canvas_screen = canvas_world:add(self.ded.scrolling)
   local window_screen = canvas_screen:add(self.ded.window_position)
   return window_screen
end

-- Take a coordinate in the canvas' screen space and convert it to
-- the window's screen space
function Editor:canvas_screen_to_window_screen(canvas_screen)
   local window_screen = canvas_screen:add(self.ded.window_position)
   return window_screen
end

function Editor:ded_load(name)
   self.ded.loaded = name
   self.ded.selected = nil
   self.ded.scrolling = tdengine.vec2(0, 0)
   self.ded.editor:SetContents('')
   
   -- Load the dialogue data itself
   local filepath = 'dialogue/' .. name
   package.loaded[filepath] = nil
   self.ded.nodes = require(filepath)

   -- Load the GUI data
   filepath = 'layouts/dialogue/' .. name
   package.loaded[filepath] = nil
   self.ded.layout_data = require(filepath)
end

function Editor:ded_short_text(node)
   local max_size = 32
   if node.kind == 'Text' or node.kind == 'Choice' then
	  if string.len(node.text) < max_size then
		 return string.sub(node.text, 0, max_size)
	  else
		 return string.sub(node.text, 0, max_size - 3) .. '...'
	  end
   elseif node.kind == 'Set' then
	  return 'set ' .. node.variable
   else
	  print('forgot to add new node type to Editor:ded_short_text(): ' .. node.kind)
   end
end

function Editor:ded_full_path()
   if string.len(self.ded.loaded) > 0 then
	  return 'src/scripts/dialogue/' .. self.ded.loaded .. '.lua'
   end

   return 'no file loaded'
end

function Editor:input_slot(id)
   local gnode = self.ded.layout_data[id]
   local canvas_world = tdengine.vec2(
	  gnode.position.x,
	  gnode.position.y + (gnode.size.y / 2))
   return self:canvas_world_to_window_screen(canvas_world)
end

function Editor:output_slot(id)
   local gnode = self.ded.layout_data[id]
   local canvas_world = tdengine.vec2(
	  gnode.position.x + gnode.size.x,
	  gnode.position.y + (gnode.size.y / 2))
   return self:canvas_world_to_window_screen(canvas_world)
end

function Editor:dialogue_editor()
   imgui.Begin('Dialogue Editor', true)

   -- Draw the sidebar
   imgui.BeginChild('sidebar', 500, 0)
   
   imgui.Text(self:ded_full_path())

   -- Save button
   if imgui.Button('Save') then
	  local serpent = require('serpent')

	  -- Save out the engine data
	  local data_path = 'src/scripts/dialogue/' .. self.ded.loaded .. '.lua'
	  data_path = tdengine.paths.absolute_path(data_path)
	  local data_file = io.open(data_path, 'w')
	  if data_file then
		 data_file:write('return ')
		 data_file:write(serpent.block(self.ded.nodes, { comment = false }))
	  else
		 print('could not open data file: ' .. data_path)
	  end

	  -- Save out the layout data
	  local layout_path = 'src/scripts/layouts/dialogue/' .. self.ded.loaded .. '.lua'
	  layout_path = tdengine.paths.absolute_path(layout_path)
	  local layout_file = io.open(layout_path, 'w')
	  if layout_file then
		 layout_file:write('return ')
		 layout_file:write(serpent.block(self.ded.layout_data, { comment = false }))
	  else
		 print('could not open gui node data: ' .. layout_path)		 
	  end
   end

   imgui.Separator()

   -- Detail view
   local selected = self.ded.nodes[self.ded.selected]
   if selected then
	  imgui.extensions.VariableName('Kind')
	  imgui.SameLine()
	  imgui.Text(selected.kind)

	  imgui.extensions.VariableName('Entity')
	  imgui.SameLine()
	  imgui.Text(selected.who)

	  imgui.PushTextWrapPos(0)
	  imgui.Text(self.ded.editor:Contents())
	  imgui.PopTextWrapPos()
   end
   
   imgui.Separator()

   -- A list of all nodes, just using their short names
   local node_hovered_in_list = nil
   if imgui.TreeNode('Nodes') then
	  for id, node in pairs(self.ded.nodes) do
		 local imid = id .. 'list_view'
		 imgui.PushID(imid)

		 -- Write the selected node in a different color
		 local pushed_color = false
		 if self.ded.selected == id then
			local hl_color = tdengine.color32(0, 255, 0, 255)
			imgui.PushStyleColor(imgui.constant.Col.Text, hl_color)
			pushed_color = true
		 end

		 -- Display the node, and if clicked select it
		 if imgui.MenuItem(self:ded_short_text(node)) then
			self.ded.selected = node.uuid
		 end

		 -- Pop the color we pushed for the selected node!
		 if pushed_color then
			imgui.PopStyleColor()
		 end
		 
		 if imgui.IsItemHovered() then
			node_hovered_in_list = node.uuid
		 end

		 imgui.PopID()
	  end
	  imgui.TreePop()
   end

   imgui.Text('scroll: ' .. self.ded.scrolling.x .. ', ' .. self.ded.scrolling.y)
   if self.inslot then
	  imgui.Text('inslot: ' .. self.inslot.x .. ', ' .. self.inslot.y)
   end
   if self.pos then
	  imgui.Text('pos: ' .. self.pos.x .. ', ' .. self.pos.y)
   end   

   imgui.EndChild() -- Sidebar

   imgui.SameLine()

   -- Canvas!
   imgui.BeginGroup()
   
   -- Set up the canvas
   imgui.PushStyleVar_2(imgui.constant.StyleVar.FramePadding, 1, 1)
   imgui.PushStyleVar_2(imgui.constant.StyleVar.WindowPadding, 0, 0)

   local bg_color = tdengine.color32(60, 60, 70, 200)
   imgui.PushStyleColor(imgui.constant.Col.ChildBg, bg_color)
   
   local flags = bitwise(tdengine.op_or, imgui.constant.WindowFlags.NoScrollbar, imgui.constant.WindowFlags.NoMove)
    
   imgui.BeginChild('scrolling_region', 0, -200, true, flags)
   self.ded.window_position = tdengine.vec2(imgui.GetCursorScreenPos())

   -- Draw the grid
   local cursor_x, cursor_y = imgui.GetCursorScreenPos()
   local offset = tdengine.vec2(
	  self.ded.scrolling.x + cursor_x,
	  self.ded.scrolling.y + cursor_y)
   local line_color = tdengine.color32(200, 200, 200, 40)
   local grid_size = 64
   local wsx, wsy = imgui.GetWindowSize()

   for off_x = math.fmod(self.ded.scrolling.x, grid_size), wsx, grid_size do
	  local top = tdengine.vec2(off_x, 0)
	  top = self:canvas_screen_to_window_screen(top)
	  
	  local bottom = tdengine.vec2(off_x, wsy)
	  bottom = self:canvas_screen_to_window_screen(bottom)
	  
	  imgui.DrawList_AddLine(top.x, top.y, bottom.x, bottom.y, line_color)
   end

   for off_y = math.fmod(self.ded.scrolling.y, grid_size), wsy, grid_size do
	  -- 
	  local left = tdengine.vec2(0, off_y)
	  left = self:canvas_screen_to_window_screen(left)
	  
	  local right = tdengine.vec2(wsx, off_y)
	  right = self:canvas_screen_to_window_screen(right)

	  imgui.DrawList_AddLine(left.x, left.y, right.x, right.y, line_color)
   end

   -- Draw nodes
   self.hovered = nil
   local node_padding = tdengine.vec2(8, 8)
   
   imgui.DrawList_ChannelsSplit(3)

   for id, node in pairs(self.ded.nodes) do
	  imgui.PushID(id)

	  -- GUI data stored separately from actual game data
	  local gnode = self.ded.layout_data[id]
	  local canvas_position = tdengine.vec2(gnode.position.x, gnode.position.y)

	  local node_rect_min = self:canvas_world_to_window_screen(canvas_position)
	  local node_contents_cursor = node_rect_min:add(node_padding)
	  
	  -- Draw the node contents
	  imgui.DrawList_ChannelsSetCurrent(2)

	  local old_any_active = imgui.IsAnyItemActive()

	  imgui.SetCursorScreenPos(node_contents_cursor:unpack())
	  imgui.BeginGroup()
	  imgui.Text(node.kind)
	  if node.kind == 'Text' then
		 imgui.Text(self:ded_short_text(node))
	  elseif node.kind == 'Set' then
		 --imgui.extensions.VariableName(node.variable)
		 --imgui.SameLine()
		 --imgui.Text(tostring(node.value))
	  elseif node.kind == 'Choice' then
		 --imgui.Text(node.text)
	  end
	  imgui.EndGroup()

	  local contents_size = tdengine.vec2(imgui.GetItemRectSize())
	  local padding_size = node_padding:scale(2)
	  gnode.size = contents_size:add(padding_size)
	  local node_rect_max = node_rect_min:add(gnode.size)
	  
	  -- Set up the 'button' that makes up the node
	  imgui.SetCursorScreenPos(node_rect_min:unpack())
	  imgui.InvisibleButton('node', gnode.size:unpack())

	  -- Figure out whether we're pressed, hovered, or dragged
	  local pressed = imgui.IsItemActive()
	  if pressed then
		 self.ded.selected = id
		 local text = ternary(node.text, node.text, node.variable)
		 self.ded.editor:SetContents(text)

		 -- If someone left clicked us, check whether they're trying to
		 -- (dis)connect themselves to you
		 if imgui.IsMouseClicked(0) then
			if self.ded.connecting then
			   local parent = self.ded.nodes[self.ded.connecting]
			   table.insert(parent.children, id)
			   self.ded.connecting = nil
			end
			if self.ded.disconnecting then
			   local parent = self.ded.nodes[self.ded.disconnecting]
			   delete(parent.children, id)
			   self.ded.disconnecting = nil
			end
		 end
		 
		 -- Pressed with left click? Drag
		 if imgui.IsMouseDragging(0) then
			local delta = tdengine.vec2(imgui.MouseDelta())
			local last_position = tdengine.vec2(gnode.position.x, gnode.position.y)
			gnode.position = last_position:add(delta)
		 end
	  end

	  -- Pressed with right click? Context menu
	  if imgui.IsItemClicked(1) then
		 imgui.OpenPopup('node_context_menu')
	  end

	  imgui.PushStyleVar_2(imgui.constant.StyleVar.WindowPadding, 8, 8)
	  if imgui.BeginPopup('node_context_menu') then
		 if imgui.MenuItem('Connect') then
			self.ded.connecting = id
		 end
		 if imgui.MenuItem('Disconnect') then
			self.ded.disconnecting = id
		 end

		 imgui.EndPopup()
	  end
	  imgui.PopStyleVar()

	  local hovered = false
	  if imgui.IsItemHovered() then
		 hovered = true
	  end
	  hovered = hovered or node_hovered_in_list == id
	  
	  if hovered then
		 self.hovered = id
	  end

	  -- Draw node background and slots
	  imgui.DrawList_ChannelsSetCurrent(1)

   
	  -- Slots
	  local radius = 8
	  local ay = average(node_rect_max.y, node_rect_min.y)

	  local in_slot_color = tdengine.color32(255, 100, 255, 255)
	  local in_slot = self:input_slot(id)
	  imgui.DrawList_AddCircleFilled(in_slot.x, in_slot.y, radius, in_slot_color)

	  local out_slot_color = tdengine.color32(100, 0, 200, 255)
	  local out_slot = self:output_slot(id)
	  imgui.DrawList_AddCircleFilled(out_slot.x, out_slot.y, radius, out_slot_color)
	  
	  -- Draw a rectangle for the node's background
	  local hl_node_color = tdengine.color32(75, 75, 75, 255)
	  local node_color = tdengine.color32(60, 60, 60, 255)
	  
	  local highlight = hovered or node.uuid == self.ded.selected
	  local color = ternary(highlight, hl_node_color, node_color)
	  local rounding = 4
	  imgui.DrawList_AddRectFilled(node_rect_min.x, node_rect_min.y, node_rect_max.x, node_rect_max.y, color, rounding)

	  imgui.PopID() -- Unique node ID
   end

   -- Draw the links between nodes
   imgui.DrawList_ChannelsSetCurrent(0)

   local link_color = tdengine.color32(200, 200, 200, 255)
   local disconnect_color = tdengine.color32(255, 0, 0, 255)
   local thickness = 2

   for id, node in pairs(self.ded.nodes) do
	  local p0 = self:output_slot(id)
	  local use_dc_prompt_color = self.ded.disconnecting == id
	  
	  local children = node.children
	  for index, child_id in pairs(children) do
		 local p1 = self:input_slot(child_id)

		 use_dc_prompt_color = use_dc_prompt_color and self.hovered == child_id
		 color = ternary(use_dc_prompt_color, disconnect_color, link_color)
		 imgui.DrawList_AddBezierCurve(
			p0.x, p0.y,
			p0.x + 50, p0.y,
			p1.x - 50, p1.y,
			p1.x, p1.y,
			color, thickness)
	  end
   end

   if self.ded.connecting then
	  local p0 = self:output_slot(self.ded.connecting)
	  local cursor = tdengine.vec2(imgui.GetMousePos())

	  imgui.DrawList_AddBezierCurve(
		 p0.x, p0.y,
		 p0.x + 50, p0.y + 50,
		 cursor.x - 50, cursor.y - 50,
		 cursor.x, cursor.y,
		 link_color, thickness)
   end

   imgui.DrawList_ChannelsMerge()

   -- Right clicking in window background brings up a menu.
   local rclick = imgui.IsMouseClicked(1)
   local in_window = imgui.IsMouseHoveringWindow()
   local on_node = imgui.IsAnyItemHovered()
   if rclick and in_window and not on_node then
	  imgui.OpenPopup('context_menu')
   end

   imgui.PushStyleVar_2(imgui.constant.StyleVar.WindowPadding, 8, 8)
   if imgui.BeginPopup('context_menu') then
	  if imgui.TreeNode('Add Node') then
		 local node = nil
		 if imgui.MenuItem('Text') then
			node = {
			   kind = 'Text',
			   text = 'lorem',
			   who = 'jerry',
			   children = {},
			   uuid = tdengine.uuid()
			}
		 end
		 if imgui.MenuItem('Choice') then
			node = {
			   kind = 'Choice',
			   text = 'ipsum',
			   who = 'pigpen',
			   children = {},
			   uuid = tdengine.uuid()
			}
		 end
		 if imgui.MenuItem('Set') then
			node = {
			   kind = 'Set',
			   variable = 'variable',
			   value = true,
			   children = {},
			   uuid = tdengine.uuid()
			}
		 end

		 if node then
			self.ded.nodes[node.uuid] = node
			self.ded.layout_data[node.uuid] = {
			   position = tdengine.vec2(100, 100),
			   size = tdengine.vec2(0, 0)
			}
		 end

		 imgui.TreePop()
	  end
	  imgui.EndPopup()
   end
   imgui.PopStyleVar()

   local canvas_hovered = imgui.IsWindowHovered()
   local middle_click = imgui.IsMouseDragging(2, 0)
   local clicked_on_node = imgui.IsAnyItemActive()

   if canvas_hovered and middle_click and not clicked_on_node then
	  local delta = tdengine.vec2(imgui.MouseDelta())
	  self.ded.scrolling = self.ded.scrolling:add(delta)
   end

   imgui.EndChild()

   -- @hack: 0 doesn't infer like I'd expect it to
   self.ded.editor:Draw(-1, -1)
   if self.ded.selected then
	  local selected = self.ded.nodes[self.ded.selected]
      selected.text = self.ded.editor:Contents()
   end


   imgui.PopStyleVar() -- FramePadding
   imgui.PopStyleVar() -- WindowPadding
   imgui.PopStyleColor() -- ChildBg
   imgui.EndGroup() -- Canvas

   imgui.End()
end

