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

  -- Text, Choice, Set, (Branch)
  self.ded_nodes = {
	 sha0 = {
		kind = 'Text',
		text = 'hi n_n',
		who = 'player',
		children = { 'sha1' },
	 },
	 sha1 = {
		kind = 'Set',
		variable = 'variable',
		value = true,
		children = { 'sha2' },
	 },
	 sha2 = {
		kind = 'Choice',
		text = 'best dead album?',
		who = 'player',
		children = { 'sha3', 'sha4' },
	 },
	 sha3 = {
		kind = 'Text',
		text = 'europe 72',
		who = 'player',
		children = {},
	 },
	 sha4 = {
		kind = 'Text',
		text = 'american beauty',
		who = 'player',
		children = {},
	 }
  }
  
  self.ded_gui = {
	 sha0 = {
		position = { x = 0, y = 0 },
		size = { x = 0, y = 0 }
	 },
	 sha1 = {
		position = { x = 100, y = 0 },
		size = { x = 0, y = 0 }
	 },
	 sha2 = {
		position = { x = 250, y = 0 },
		size = { x = 0, y = 0 }
	 },
	 sha3 = {
		position = { x = 400, y = 0 },
		size = { x = 0, y = 0 }
	 },
	 sha4 = {
		position = { x = 400, y = 74 },
		size = { x = 0, y = 0 }
	 }
  }

  self.ded_selected = ''
  self.ded_scrolling = { x = 0, y = 0 }
  
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

function Editor:ded_load(name)
   print('ded load: ' .. name)
end

function Editor:ded_save()
end

function Editor:dialogue_editor()
   imgui.Begin('Dialogue Editor', true)

   imgui.BeginGroup()
   
   -- Set up the canvas
   imgui.PushStyleVar_2(imgui.constant.StyleVar.FramePadding, 1, 1)
   imgui.PushStyleVar_2(imgui.constant.StyleVar.WindowPadding, 0, 0)

   local bg_color = tdengine.color32(60, 60, 70, 200)
   imgui.PushStyleColor(imgui.constant.Col.ChildBg, bg_color)
   
   local flags = bitwise(tdengine.op_or, imgui.constant.WindowFlags.NoScrollbar, imgui.constant.WindowFlags.NoMove)
   imgui.BeginChild('scrolling_region', 0, 0, true, flags)
   
   imgui.PushItemWidth(120)

   -- Draw the grid
   local cursor_x, cursor_y = imgui.GetCursorScreenPos()
   local offset = tdengine.vec2(
	  self.ded_scrolling.x + cursor_x,
	  self.ded_scrolling.y + cursor_y)
   --local lclr = 0xc6c6c6028
   local lclr = tdengine.color32(200, 200, 200, 40)--0x28c6c6c6
   local grid_size = 64
   local wsx, wsy = imgui.GetWindowSize()
   local wpx, wpy = imgui.GetCursorScreenPos() -- Cursor doesn't mean your mouse. It's the ImGui widget cursor.

   for off_x = math.modf(self.ded_scrolling.x, grid_size), wsx, grid_size do
	  -- Bottom
	  local a = {
		 x = wpx + off_x,
		 y = wpy }
	  -- Top
	  local b = {
		 x = wpx + off_x,
		 y = wpy + wsy }		 
	  imgui.DrawList_AddLine(a.x, a.y, b.x, b.y, lclr)
   end

   for off_y = math.modf(self.ded_scrolling.y, grid_size), wsy, grid_size do
	  -- Left
	  local a = {
		 x = wpx,
		 y = wpy + off_y }
	  -- Right
	  local b = {
		 x = wpx + wsx,
		 y = wpy + off_y }		 
	  imgui.DrawList_AddLine(a.x, a.y, b.x, b.y, lclr)
   end

   -- Draw nodes
   local node_padding = tdengine.vec2(8, 8)
   imgui.DrawList_ChannelsSplit(2)
   for sha, node in pairs(self.ded_nodes) do
	  -- GUI data stored separately from actual game data
	  local gnode = self.ded_gui[sha]
	  
	  imgui.PushID(sha)

	  local node_rect_min = offset:add(gnode.position)
	  local node_contents_cursor = node_rect_min:add(node_padding)
	  
	  -- Draw the node contents
	  imgui.DrawList_ChannelsSetCurrent(1)

	  local old_any_active = imgui.IsAnyItemActive()

	  imgui.SetCursorScreenPos(node_contents_cursor:unpack())
	  imgui.BeginGroup()
	  imgui.Text(node.kind)
	  if node.kind == 'Text' then
		 imgui.Text(node.text)
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
	  
	  -- Draw the node background
	  imgui.DrawList_ChannelsSetCurrent(0)
	  imgui.SetCursorScreenPos(node_rect_min:unpack())
	  imgui.InvisibleButton('node', gnode.size:unpack())
	  
	  local hovered = false
	  if imgui.IsItemHovered() then
		 hovered = true
		 open_context_menu = true
	  end
	  
	  local pressed = imgui.IsItemActive()
	  if pressed then
		 self.ded_selected = sha
		 local text = ternary(node.text, node.text, node.variable)
		 
		 if imgui.IsMouseDragging(0) then
			local delta = tdengine.vec2(imgui.MouseDelta())
			local last_position = tdengine.vec2(gnode.position.x, gnode.position.y)
			gnode.position = last_position:add(delta)
		 end
	  end

	  local node_color = tdengine.color32(75, 75, 75, 255)
	  local hl_node_color = tdengine.color32(60, 60, 60, 255)
	  local color = ternary(hovered, node_color, hl_node_color)
	  imgui.DrawList_AddRectFilled(node_rect_min.x, node_rect_min.y, node_rect_max.x, node_rect_max.y, color, 4)

	  imgui.PopID() -- Unique node ID
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
	  imgui.Text('sup')
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
			self.ded_nodes[node.uuid] = node
			self.ded_gui[node.uuid] = {
			   position = tdengine.vec2(100, 100),
			   size = tdengine.vec2(0, 0)
			}
		 end

		 imgui.TreePop()
	  end
	  imgui.EndPopup()
   end
   imgui.PopStyleVar()
   
   imgui.EndChild()
   imgui.PopStyleVar() -- FramePadding
   imgui.PopStyleVar() -- WindowPadding
   imgui.PopStyleColor() -- ChildBg
   imgui.EndGroup()
   imgui.End()

   -- Draw the detail view for the selected node
   imgui.Begin('ded', true)
   local selected = self.ded_nodes[self.ded_selected]
   if selected then
	  imgui.Text(selected.kind)
   end
   imgui.End()

end

