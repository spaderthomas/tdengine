local GLFW = require('glfw')
local inspect = require('inspect')

local EditState = {
  Idle = 'Idle',
  Selected = 'Selected',
  HoldingSelection = 'HoldingSelection', 
  RectangleSelect = 'RectangleSelect',
  ReadyToDrawGeometry = 'ReadyToDrawGeometry',
  DrawingGeometry = 'DrawingGeometry',
  Resizing = 'Resizing',
  MovingAabb = 'MovingAabb',
  ClickPosition = 'ClickPosition',
}

Editor = tdengine.entity('Editor')
function Editor:init(params)
  self:persist()
  
  self.options = {
	show_bounding_boxes = false,
	show_minkowksi = false,
	show_imgui_demo = false,
	show_framerate = false
  }
  
  self.state = EditState.Idle
  
  self.selected = nil
  self.destroy_selected = false
  self.position_when_selected = tdengine.vec2(0, 0)
  self.hovered = nil
  
  -- Stuff we use to add an entity to the level
  self.component_input_id = '##add_entity_component_input'
  self.param_input_id = '##add_entity_param_input'
  self.geometry = {
	origin = tdengine.vec2(),
	extents = tdengine.vec2()
  }
  self.use_geometry = false
  
  self.ded = {
	nodes = {},
	layout_data = {},
	loaded = '',
	selected = nil,
	connecting = nil,
	disconnecting = nil,
	deleting = nil,
	scrolling = tdengine.vec2(0, 0),
	window_position = tdengine.vec2(0, 0),
	input_id = '##ded_editor',
	set_who_id = '##ded:detail:set_entity'
  }
  
  -- Stored as screen coordinates, converted to world when we submit the geometry
  self.last_click = { x = 0, y = 0 }
  
  self.filter = imgui.TextFilter.new()
  self.id_filter = imgui.TextFilter.new()
  self.state_filter = imgui.TextFilter.new()

  self.display_framerate = 0
  self.average_framerate = 0
  self.frame = 0

  self:should_save(false)

  local input = self:get_component('Input')
  input:set_channel(tdengine.InputChannel.Editor)
  input:enable()
end

function Editor:update(dt)
  -- If we reloaded the scene or something, our handle is invalid
   if self.selected and not self.selected.alive then
	self.selected = nil
  end

  self:calculate_framerate()
  
  self:handle_input()
  
  imgui.SetNextWindowSize(300, 300)
  imgui.Begin('engine', true)
  imgui.Text('frame: ' .. tostring(self.frame))
  imgui.Text('fps: ' .. tostring(self.display_framerate))
  
  local screen_size = tdengine.screen_dimensions()
  imgui.extensions.Vec2('screen size', screen_size)

  local cursor = tdengine.vec2(tdengine.cursor()):truncate(3)
  imgui.extensions.Vec2('cursor (screen)', cursor)
  imgui.extensions.Vec2('cursor (world)', tdengine.screen_to_world(cursor))

  imgui.Text('editor state: ' .. self.state)

  if imgui.Button('set up rendering test') then
	 local position = tdengine.vec2(0, 0)
	 for i = 1, 1000 do
		local data = {
		   components = {
			  Position = { world = position }
		   }
		}
		local entity = tdengine.create_entity('Oliver', data)

		position.x = position.x + .1
		if position.x > .9 then
		   position.x = 0
		   position.y = position.y + .1
		end
	 end
  end

  if imgui.Button('tear down rendering test') then
	 for id, entity in pairs(tdengine.entities) do
		if entity:get_name() == 'Oliver' then
		   tdengine.destroy_entity(id)
		end
	 end
  end

  local olivers = 0
  for id, entity in pairs(tdengine.entities) do
	 if entity:get_name() == 'Oliver' then
		olivers = olivers + 1
	 end
  end
  imgui.Text('olivers: ' .. tostring(olivers))

  self:state_viewer()
  self:scene_viewer()
  self:cutscene_viewer()
  
  imgui.End() -- dashboard

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
	  local new_position = self.position_when_selected:add(diff)
	  tdengine.teleport_entity(self.selected:get_id(), new_position.x, new_position.y)
	else
	  self.state = EditState.Selected
	end
  end

  if self.destroy_selected then
	tdengine.destroy_entity(self.selected:get_id())
	self.selected = nil
	self.destroy_selected = false
  end
end

function Editor:do_geometry()
  local input = self:get_component('Input')

  if self.state == EditState.ReadyToDrawGeometry then
	if input:was_pressed(GLFW.Keys.MOUSE_BUTTON_1) then
	  self.last_click = tdengine.cursor()
	  self.state = EditState.DrawingGeometry
	end
  elseif self.state == EditState.DrawingGeometry then
	if input:is_down(GLFW.Keys.MOUSE_BUTTON_1) then
	  local rect = self:get_mouse_rect()
	  tdengine.draw.rect_outline_screen(rect, tdengine.colors.red)
	else
	  local rect = self:get_mouse_rect()
	  self.geometry = {
		origin = tdengine.screen_to_world(rect.origin),
		extents = rect.extents
	  }

	  self.state = EditState.Idle
	  self.last_click = tdengine.vec2(0, 0)
	end
  elseif self.state == EditState.Resizing then
	local box = self.selected:get_component('BoundingBox')
	local position = self.selected:get_component('Position')
	if not box or not position then
	  self.state = EditState.Idle -- should never happen
	  return
	end

	local cursor = tdengine.screen_to_world(tdengine.cursor())
	box.extents = cursor:subtract(position.world):abs():scale(2)

	if input:was_pressed(GLFW.Keys.MOUSE_BUTTON_1) then
	  tdengine.register_collider(self.selected:get_id())
	  
	  self.state = EditState.Idle
	  self.last_click = tdengine.vec2(0, 0)
	end
  elseif self.state == EditState.MovingAabb then
	local box = self.selected:get_component('BoundingBox')
	local position = self.selected:get_component('Position')
	if not box or not position then
	  self.state = EditState.Idle -- should never happen
	  return
	end

	local cursor = tdengine.screen_to_world(tdengine.cursor())
	local world = tdengine.vec2(position.world)
	box.offset = cursor:subtract(world)

	if input:was_pressed(GLFW.Keys.MOUSE_BUTTON_1) then
	  tdengine.register_collider(self.selected:get_id())
	  
	  self.state = EditState.Idle
	  self.last_click = tdengine.vec2(0, 0)
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
	if self.selected then
	  local graphic = self.selected:get_component('Graphic')
	  if graphic ~= nil then
		graphic.flags = 0
	  end
	end
	self.selected = nil
	
	self.state = EditState.Idle
  end

  input:set_channel(tdengine.InputChannel.All)
  if input:was_pressed(GLFW.Keys.LEFT_ALT) then
    tdengine.internal.toggle_console()
  end
  input:set_channel(tdengine.InputChannel.Editor)

  if input:was_pressed(GLFW.Keys.F1) then
	tdengine.step_mode()
  end
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

  local set_selected_draw_flags = function(flags)
	if self.selected ~= nil then
	  local graphic = self.selected:get_component('Graphic')
	  if graphic ~= nil then
		graphic.flags = flags
	  end
	end
  end
  
  local left = input:was_pressed(GLFW.Keys.MOUSE_BUTTON_1)
  local right = input:was_pressed(GLFW.Keys.MOUSE_BUTTON_2)
  if left or right then
	-- Un-highlight the last selected thing
	set_selected_draw_flags(tdengine.render_flags.none)

	-- Ray cast to see if we clicked anything...
	local cursor = tdengine.screen_to_world(tdengine.cursor())
	self.selected = tdengine.ray_cast(cursor.x, cursor.y)

	-- If we didn't, reset
	if not self.selected then
	  self.state = EditState.Idle
	end

	set_selected_draw_flags(tdengine.render_flags.highlighted)
  end
  
  if left then
	if self.selected ~= nil then
	  self.state = EditState.HoldingSelection
	  self.last_click = tdengine.vec2(tdengine.cursor())

	  -- Mark down where it was when we clicked it, so we can drag it
	  local position = self.selected:get_component('Position')
	  if position ~= nil then
		self.position_when_selected = tdengine.vec2(
		  position.world.x,
		  position.world.y)
	  end
	end
	-- Right click brings up a context menu (either for this entity or in general)
  elseif right then
	if self.selected then imgui.OpenPopup('entity_context_menu')
	else imgui.OpenPopup('main_context_menu') end
  end

  imgui.PushStyleVar_2(imgui.constant.StyleVar.WindowPadding, 8, 8)
  if imgui.BeginPopup('entity_context_menu') then
	if imgui.MenuItem('Delete') then
	  self.destroy_selected = true
	end

	local box = self.selected:get_component('BoundingBox')
	if box then
	  if imgui.MenuItem('Resize AABB') then
		self.state = EditState.Resizing
		self.last_click = tdengine.vec2(tdengine.cursor())
	  end
	  if imgui.MenuItem('Move AABB') then
		self.state = EditState.MovingAabb
		self.last_click = tdengine.vec2(tdengine.cursor())
	  end
	end
	
	imgui.EndPopup()
  end
  imgui.PopStyleVar()

  imgui.PushStyleVar_2(imgui.constant.StyleVar.WindowPadding, 8, 8)
  if imgui.BeginPopup('main_context_menu') then
	if imgui.MenuItem('Add Entity') then
	  imgui.OpenPopup('dogdog')
	end
	imgui.EndPopup()
  end
  imgui.PopStyleVar()
end

function Editor:entity_viewer()
  self.filter:Draw("Filter by name")
  local entity_hovered_in_list = nil

  for id, entity in pairs(tdengine.entities) do
	local name = entity:get_name()
	local sid = tostring(id)
	if self.filter:PassFilter(name) or self.filter:PassFilter(sid) then
	  imgui.PushID(id .. '##list_view')

	  -- Write the selected node in a different color
	  local pushed_color = false
	  if self.selected and self.selected:get_id() == id then
		local hl_color = tdengine.color32(0, 255, 0, 255)
		imgui.PushStyleColor(imgui.constant.Col.Text, hl_color)
		pushed_color = true
	  end

	  -- Display the node, and if clicked select it
	  if imgui.MenuItem(name) then
		self.selected = entity
	  end

	  -- Pop the color we pushed for the selected node!
	  if pushed_color then
		imgui.PopStyleColor()
	  end
	  
	  if imgui.IsItemHovered() then
		entity_hovered_in_list = id
	  end

	  imgui.PopID()
	end
  end
end

function Editor:selected_entity()
  if self.selected ~= nil then
	
	-- if self.selected ~= nil then
	-- 	 imgui.extensions.Entity(self.selected, '##editor:selected')
	-- end

	imgui.extensions.Entity(self.selected)
  end
end

function Editor:get_mouse_rect()
  local cursor = tdengine.cursor()
  return {
	extents = {
	  x = math.abs(cursor.x - self.last_click.x),
	  y = math.abs(cursor.y - self.last_click.y)
	},
	origin = {
	  x = (cursor.x + self.last_click.x) / 2,
	  y = (cursor.y + self.last_click.y) / 2
	}
  }
end

function Editor:get_mouse_vector()
  local cursor = tdengine.vec2(tdengine.cursor())
  return cursor:subtract(self.last_click)
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

function Editor:scene_viewer()
  imgui.Begin("scene", true)
  imgui.Text(self:scene_descriptor())

  local button_size = { x = 150, y = 0 }
  local loaded = false
  
  if imgui.Button('Save To Memory', button_size.x, button_size.y) then
	 tdengine.save_current_scene_to_memory()
  end

  local id = '##scene_viewer:load_from_memory'
  if imgui.Button('Load From Memory', button_size.x, button_size.y) then
	tdengine.load_scene_from_memory(imgui.InputTextContents(id))
	loaded = true
  end
  imgui.SameLine()
  imgui.InputText(id, 63)

  local id = '##scene_viewer:save_to_template'
  if imgui.Button('Save To Template', button_size.x, button_size.y) then
	local which = imgui.InputTextContents(id)
	if which:len() == 0 then which = tdengine.loaded_scene.name end
	tdengine.save_current_scene_as_template(which)
  end
  imgui.SameLine()
  imgui.InputText(id, 63)

  local id = '##scene_viewer:load_from_template'
  if imgui.Button('Load From Template', button_size.x, button_size.y) then
	tdengine.load_scene_from_template(imgui.InputTextContents(id))
	loaded = true
  end
  imgui.SameLine()
  imgui.InputText(id, 63)

  local id = '##scene_viewer:save_to_disk'
  if imgui.Button('Save To Disk', button_size.x, button_size.y) then
	tdengine.save_current_scene_to_disk(imgui.InputTextContents(id))
  end
  imgui.SameLine()
  imgui.InputText(id, 63)

  local id = '##scene_viewer:load_from_disk'
  if imgui.Button('Load From Disk', button_size.x, button_size.y) then
	tdengine.load_scene_from_disk(imgui.InputTextContents(id))
	loaded = true
  end
  imgui.SameLine()
  imgui.InputText(id, 63)

  if loaded then self.selected = nil end

  imgui.Separator()

  if imgui.TreeNode('Scenes in Memory') then
	for scene_name, scene_data in pairs(tdengine.scenes) do
	  local scenes_in_memory_id = '##scene_viewer:scenes_in_memory:' .. scene_name
	  local scene_node_id = scene_name .. scenes_in_memory_id
	  
	  if imgui.TreeNode(scene_node_id) then
		local entities = scene_data.entities
		for index, entity in pairs(entities) do
		  local entity_node_id = entity.name .. scenes_in_memory_id .. ':' .. tostring(index)
		  entity_node_id = entity_node_id .. ':' .. entity.name
		  imgui.extensions.Table(entity_node_id, entity)
		end
		imgui.TreePop()
	  end
	end
	imgui.TreePop()
  end

  imgui.Separator()

  if imgui.TreeNode('Add Entity') then
	local name_input = '##add_entity_name_input'
	imgui.Text('put the entity name here')
	imgui.InputText(name_input, 63)

	local buffer_size = 512
	local size = tdengine.vec2(328, 200)
	
	imgui.Text('write a table of components to use to load the entity')
	imgui.InputTextMultiline(self.component_input_id, buffer_size, size.x, size.y)

	imgui.Text('some special tools to autogenerate component data')
	if imgui.Button('Autogenerate Bounding Box') then
	  self.state = EditState.ReadyToDrawGeometry
	  self.use_geometry = true
	end

	
	imgui.Text('write a table of parameters to pass to the entity')
	imgui.InputTextMultiline(self.param_input_id, buffer_size, size.x, size.y)

	local invalid_messages = {}
	local name = imgui.InputTextContents(name_input)
	if #name == 0 then
	  local message = 'You need to specify which entity to create'
	  table.insert(invalid_messages, message)
	end

	if #invalid_messages > 0 then
	  local color = tdengine.color32(255, 0, 0, 255)
	  imgui.PushStyleColor(imgui.constant.Col.Text, color)
	else
	  local color = tdengine.color32(0, 255, 0, 255)
	  imgui.PushStyleColor(imgui.constant.Col.Text, color)
	end
	if imgui.Button('Add Entity') then
	  -- Load in the text from the component input field as a table
	  local text = imgui.InputTextContents(self.component_input_id)
	  text = ternary(text:len() == 0, '{}', text)
	  text = 'return ' .. text
	  
	  load_components, lc_message = loadstring(text)

	  -- Load in the text from the param input field as a table
	  text = imgui.InputTextContents(self.param_input_id)
	  text = ternary(text:len() == 0, '{}', text)
	  text = 'return ' .. text
	  
	  load_params, lp_message = loadstring(text)

	  if load_components and load_params then
		local components = load_components()
		local params = load_params()

		-- Create a table which we'll use to load the entity. The data in this
		-- table is the same as any other saved entity. There's nothing special!
		data = {
		  name = imgui.InputTextContents(name_input),
		  components = components,
		  params = params
		}

		-- It's more convenient to draw a box and use that for the position and
		-- AABB, so inject that data if we did that
		if self.use_geometry then
		  data.components.BoundingBox = {
			extents = self.geometry.extents,
			offset = tdengine.vec2(0, 0)
		  }
		  data.components.Position = {
			world = self.geometry.origin
		  }
		end

		-- Create the entity and load it with the data we just made
		local id = tdengine.create_entity(data.name, data)
		if not id then
		  print('something went wrong calling create_entity()')
		  return
		end
		
		self.use_geometry = false
		self.geometry = {
		  origin = tdengine.vec2(0, 0),
		  extents = tdengine.vec2(0, 0) }

		-- loadstring() returns nil if there's a syntax error
	  else
		print('scene_viewer(): syntax error when adding entity:')
		print(lc_message)
		print(lp_message)
		return
	  end

	end
	
	imgui.PopStyleColor()

	for index, message in pairs(invalid_messages) do
	  imgui.Text(message)
	end

	imgui.TreePop()
  end
  
  imgui.Separator()
  self:entity_viewer()
  imgui.Separator()
  
  self:selected_entity()
  imgui.Separator()
  
  imgui.End() -- scene

end

function Editor:scene_descriptor()
  if tdengine.loaded_scene.path and tdengine.loaded_scene.path:len() > 0 then
	return 'src/scripts/' .. tdengine.loaded_scene.path .. '.lua'
  end

  return 'no scene loaded'
end

function Editor:make_dialogue_node(kind)
  local node = {
	kind = kind,
	is_entry_point = false,
	children = {},
	uuid = tdengine.uuid()
  }
  if kind == 'Text' then
	node.text = ''
	node.who = ''
  end
  if kind == 'Choice' then
	node.text = ''
  end
  if kind == 'Set' then
	node.variable = 'buns'
	node.value = true
  end
  return node

end

function Editor:ded_load(name)
  self.ded.loaded = name
  self.ded.selected = nil
  self.ded.connecting = nil
  self.ded.disconnecting = nil
  self.ded.deleting = nil
  self.ded.scrolling = tdengine.vec2(0, 0)
  imgui.InputTextSetContents(self.ded.input_id, '')

  self.ded.nodes = tdengine.load_dialogue(name)
  if not self.ded.nodes then
	self.ded.nodes = {}
	return
  end
  
  -- Load the GUI data
  filepath = 'layouts/dialogue/' .. name
  package.loaded[filepath] = nil
  status, self.ded.layout_data = pcall(require, filepath)
  if not status then
	self.ded.nodes = {}
	
	local message = 'ded_load(): could not find GUI layout. '
	message = message .. 'dialogue was ' .. filepath
	tdengine.log(message, tdengine.log_flags.default)
	return
  end
end

function Editor:ded_save(name)
  local serpent = require('serpent')

  -- Save out the engine data
  local data_path = 'src/scripts/dialogue/' .. name .. '.lua'
  data_path = tdengine.paths.absolute(data_path)
  local data_file = io.open(data_path, 'w')
  if data_file then
	data_file:write('return ')
	data_file:write(serpent.block(self.ded.nodes, { comment = false }))
	data_file:close()
  else
	print('ded_save(): could not open data file: ' .. data_path)
  end

  -- Save out the layout data
  local layout_path = 'src/scripts/layouts/dialogue/' .. name .. '.lua'
  layout_path = tdengine.paths.absolute(layout_path)
  local layout_file = io.open(layout_path, 'w')
  if layout_file then
	layout_file:write('return ')
	layout_file:write(serpent.block(self.ded.layout_data, { comment = false }))
	layout_file:close()
  else
	print('ded_save(): could not open gui node data: ' .. layout_path)		 
  end
end

function Editor:ded_new(name)
  if not name then
	print('ded_new(): no name')
	return
  end
  if #name == 0 then
	print('ded_new(): empty name')
	return
  end
  local serpent = require('serpent')
  local empty = {}

  -- Save out the engine data
  local data_path = 'src/scripts/dialogue/' .. name .. '.lua'
  data_path = tdengine.paths.absolute(data_path)
  local data_file = io.open(data_path, 'w')
  if data_file then
	data_file:write('return ')
	data_file:write(serpent.block(empty, { comment = false }))
	data_file:close()
  else
	print('ded_new(): could not open data file: ' .. data_path)
  end

  -- Save out the layout data
  local layout_path = 'src/scripts/layouts/dialogue/' .. name .. '.lua'
  layout_path = tdengine.paths.absolute(layout_path)
  local layout_file = io.open(layout_path, 'w')
  if layout_file then
	layout_file:write('return ')
	layout_file:write(serpent.block(empty, { comment = false }))
	layout_file:close()
  else
	print('ded_save(): could not open gui node data: ' .. layout_path)		 
  end

  self:ded_load(name)
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

function Editor:ded_select(id, node)
  self.ded.selected = id

  if node.who then
	imgui.InputTextSetContents(self.ded.set_who_id, node.who)
  end

  local text = ternary(node.text, node.text, node.variable)
  if node.kind == 'Text' or node.kind == 'Choice' then
	imgui.InputTextSetContents(self.ded.input_id, text)
  else
	imgui.InputTextSetContents(self.ded.input_id, '')
  end
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
  imgui.Begin('dialogue', true)

  -- Draw the sidebar
  imgui.BeginChild('sidebar', 500, 0)
  
  imgui.Text(self:ded_full_path())

  -- Buttons: Save, Save As, Load, New
  local button_size = { x = 100, y = 0 }

  if imgui.Button('Save', button_size.x, button_size.y) then
	self:ded_save(self.ded.loaded)
  end

  local id = '##ded_save_as'
  if imgui.Button('Save As', button_size.x, button_size.y) then
	self:ded_save(imgui.InputTextContents(id))
  end
  imgui.SameLine()
  imgui.InputText(id, 63)

  id = '##ded_load'
  if imgui.Button('Load', button_size.x, button_size.y) then
	-- Because you can still click this and have the grid hidden
	tdengine.use_layout('ded') 

	self:ded_load(imgui.InputTextContents(id))
	imgui.InputTextClear(id)
  end
  imgui.SameLine()
  imgui.InputText(id, 63)

  id = '##ded_new'
  if imgui.Button('New', button_size.x, button_size.y) then
	-- Because you can still click this and have the grid hidden
	tdengine.use_layout('ded') 

	self:ded_new(imgui.InputTextContents(id))
	imgui.InputTextClear(id)
  end
  imgui.SameLine()
  imgui.InputText(id, 63)

  imgui.Separator()
  
  -- @ded:detail
  local selected = self.ded.nodes[self.ded.selected]
  if selected then
	imgui.extensions.VariableName('kind')
	imgui.SameLine()
	imgui.Text(selected.kind)

	imgui.extensions.VariableName('uuid')
	imgui.SameLine()
	imgui.Text(selected.uuid)

	if selected.kind == 'Text' then
	  imgui.extensions.VariableName('who')
	  imgui.SameLine()
	  imgui.InputText(self.ded.set_who_id, 64)

	  selected.who = imgui.InputTextContents(self.ded.set_who_id)
	end
	
	if selected.kind == 'Set' then
	  imgui.Text('state: ')
	  imgui.SameLine()
	  local unique_id = '##variableinput_' .. selected.uuid
	  if imgui.InputText(unique_id, 63) then
		selected.variable = imgui.InputTextContents(unique_id)
	  end

	  imgui.Text('value: ')
	  imgui.SameLine()
	  unique_id = '##valueinput_' .. selected.uuid
	  if imgui.InputText(unique_id, 63) then
		local value = imgui.InputTextContents(unique_id)
		if value == 'true' then selected.value = true 
		elseif value == 'false' then selected.value = false end
	  end
	end

	if selected.kind == 'Text' or selected.kind == 'Choice' then
	  imgui.extensions.VariableName('text')
	  imgui.SameLine()

	  imgui.PushTextWrapPos(0)
	  imgui.Text(imgui.InputTextContents(self.ded.input_id))
	  imgui.PopTextWrapPos()
	end
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
		self:ded_select(id, node)
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
  
  if self.inslot then
	imgui.Text('inslot: ' .. self.inslot.x .. ', ' .. self.inslot.y)
  end
  if self.pos then
	imgui.Text('pos: ' .. self.pos.x .. ', ' .. self.pos.y)
  end   

  imgui.Separator()
  
  local active = false
  local done = false
  local point = 0
  local max_point = 0
  local text_box = tdengine.find_entity('TextBox')
  if text_box then
	active = text_box.active
	done = text_box.done
	point = text_box.point
	max_point = text_box.max_point
  end

  imgui.Text('active: ' .. tostring(active))
  imgui.Text('done: ' .. tostring(done))
  imgui.Text('point: ' .. tostring(point))
  imgui.Text('max_point: ' .. tostring(max_point))
  
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
	  imgui.Text(self:ded_short_text(node))
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
	  self:ded_select(id, node)

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
	  if imgui.MenuItem('Set as entry point') then
		node.is_entry_point = true
	  end
	  if imgui.MenuItem('Delete') then
		self.ded.deleting = id
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

  if self.ded.deleting then
	for id, node in pairs(self.ded.nodes) do
	  delete(node.children, self.ded.deleting)
	end
	
	self.ded.selected = ternary(self.ded.selected == self.ded.deleting, nil, self.ded.selected)
	self.ded.connecting = ternary(self.ded.connecting == self.ded.deleting, nil, self.ded.selected)
	self.ded.disconnecting = ternary(self.ded.disconnecting == self.ded.deleting, nil, self.ded.selected)

	self.ded.nodes[self.ded.deleting] = nil
	self.ded.deleting = nil
  end
  

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
		node = self:make_dialogue_node('Text')
	  end
	  if imgui.MenuItem('Choice') then
		node = self:make_dialogue_node('Choice')
	  end
	  if imgui.MenuItem('Set') then
		node = self:make_dialogue_node('Set')
	  end

	  if node then
		self.ded.nodes[node.uuid] = node

		local mouse = tdengine.vec2(imgui.GetMousePos())
		self.ded.layout_data[node.uuid] = {
		  position = mouse:subtract(self.ded.window_position):subtract(self.ded.scrolling),
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
  imgui.InputTextMultiline(self.ded.input_id, 512, -1, -1)
  if self.ded.selected then
	local selected = self.ded.nodes[self.ded.selected]
	if selected.kind == 'Text' or selected.kind == 'Choice' then
	  selected.text = imgui.InputTextContents(self.ded.input_id)
	end
  end


  imgui.PopStyleVar() -- FramePadding
  imgui.PopStyleVar() -- WindowPadding
  imgui.PopStyleColor() -- ChildBg
  imgui.EndGroup() -- Canvas

  imgui.End()
end

function Editor:state_viewer()
  imgui.Begin('state')
  self.state_filter:Draw('Filter state variables')
  local variables = {}
  for name, value in pairs(tdengine.state) do
	table.insert(variables, name)
  end
  table.sort(variables)
  for index, name in pairs(variables) do
	if self.state_filter:PassFilter(name) then 
	  imgui.Text(name .. ': ')
	  imgui.SameLine()
	  
	  local value = tdengine.state[name]
	  local true_color = tdengine.color32(0, 255, 0, 255)
	  local false_color = tdengine.color32(255, 0, 0, 255)
	  local color = ternary(value, true_color, false_color)
	  imgui.PushStyleColor(imgui.constant.Col.Text, color)
	  
	  local label = tostring(tdengine.state[name]) .. '##' .. tostring(index)
	  if imgui.Button(label) then
		tdengine.state[name] = not value
	  end
	  
	  imgui.PopStyleColor()
	end
  end
  imgui.End('state')
end


function Editor:cutscene_descriptor()
  if tdengine.active_cutscene then
	return 'src/scripts/cutscenes/' .. tdengine.active_cutscene.name .. '.lua'
  end

  return 'no cutscene active'
end

function Editor:cutscene_viewer()
  imgui.Begin('cutscene')
  imgui.Text(self:cutscene_descriptor())

  if tdengine.active_cutscene then
	for index, action in pairs(tdengine.active_cutscene.actions) do
	  local id = '##cutscene_viewer:' .. tostring(index)
	  imgui.PushID(id)
	  
	  if action.name == 'Compound' then
		if imgui.TreeNode('Compound' .. id) then
		  for child_index, child_action in pairs(action.actions) do
			local id = '##cutscene_viewer:nested:' .. tostring(child_index)
			imgui.extensions.Table(child_action.name, child_action, {}, id)
		  end
		  imgui.TreePop()
		end
	  else
		imgui.extensions.Table(action.name, action)
	  end
	  imgui.PopID()
	end
  end
  
  imgui.End()
end
