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
  self.options = {
	show_imgui_demo = false,
  }
  
  self.state = EditState.Idle
  
  self.selected = nil
  self.destroy_selected = false
  self.position_when_selected = tdengine.vec2(0, 0)
  self.hovered = nil
  self.following_player = false
  
  -- Stuff we use to add an entity to the level
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
	scroll_per_second = 100,
	window_position = tdengine.vec2(0, 0),
	input_id = '##ded_editor',
	text_who_id = '##ded:detail:set_entity',
	set_var_id = '##ded:detail:set_var',
	set_val_id = '##ded:detail:set_val',
	branch_on_id = '##ded:detail:set_branch_var',
	branch_val_id = '##ded:detail:set_branch_val',
	empty_name_id = '##ded:detail:set_empty_name',
  }

  self.entity_creator = {
	entity_cbox_id = '##ec:entity_combo_box',
	comp_cbox_id = '##ec:component_combo_box',
	selected_entity = '',
	selected_comp = '',
	components = {},
	params = {},
  }
  self.param_editor = imgui.extensions.TableEditor(self.entity_creator.params)
  self.component_editors = {}
  self.entity_editor = nil
  
  -- Stored as screen coordinates, converted to world when we submit the geometry
  self.last_click = { x = 0, y = 0 }
  
  self.filter = imgui.TextFilter.new()
  self.id_filter = imgui.TextFilter.new()
  self.state_filter = imgui.TextFilter.new()

  self.display_framerate = 0
  self.average_framerate = 0
  self.frame = 0

  self:should_save(false)
  self:persist()

  tdengine.create_entity('Player', {})

  local input = self:get_component('Input')
  input:set_channel(tdengine.InputChannel.Editor)
  input:enable()
end

function Editor:update(dt)
  -- If we reloaded the scene or something, our handle is invalid
  if self.selected and not self.selected.tdengine.alive then
	self:select_entity(nil)
  end

  tdengine.set_imgui_demo(tdengine.state['engine:imgui_demo'])

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

  local camera = tdengine.vec2(tdengine.camera()):truncate(3)
  imgui.extensions.Vec2('camera', camera)
  
  local mask = tdengine.input_mask or 0
  imgui.extensions.VariableName('input mask')
  imgui.SameLine()
  imgui.Text(string.format('%x', mask))

  mask = tdengine.old_input_mask or 0
  imgui.extensions.VariableName('old input mask')
  imgui.SameLine()
  imgui.Text(string.format('%x', mask))
  
  imgui.Text('editor state: ' .. self.state)
  
  self:state_viewer()
  self:scene_viewer()
  self:cutscene_viewer()
  
  imgui.End() -- dashboard

  self:do_geometry()

  self:dialogue_editor(dt)
  
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
	self:select_entity(nil)
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
	  if self.selected:has_component('Graphic') then
		local graphic = self.selected:get_component('Graphic')
		graphic.flags = 0
	  end
	end
	self:select_entity(nil)
	
	self.state = EditState.Idle
  end

  input:set_channel(tdengine.InputChannel.All)
  if input:was_pressed(GLFW.Keys.RIGHT_ALT) then
    tdengine.toggle_console()
  end

  if input:chord(GLFW.Keys.ALT, GLFW.Keys.J) then
	 tdengine.previous_layout()
  end
  if input:chord(GLFW.Keys.ALT, GLFW.Keys.L) then
	 tdengine.next_layout()
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

  tdengine.move_camera_by_offset(offset.x, offset.y)
end

function Editor:check_for_new_selection()
  local input = self:get_component('Input')

  local set_selected_draw_flags = function(flags)
	if self.selected ~= nil then
	  if self.selected:has_component('Graphic') then
		local graphic = self.selected:get_component('Graphic')
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
	self:select_entity(tdengine.ray_cast(cursor.x, cursor.y))

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
end

function Editor:draw_entity_creator()
  -- Entity creator!
  -- Combo box to choose your entity type
  if imgui.BeginCombo(self.entity_creator.entity_cbox_id, self.entity_creator.selected_entity) then
	for name, _ in pairs(tdengine.entity_types) do
	  if imgui.Selectable(name) then
		self.entity_creator.selected_entity = name
	  end
	end
	imgui.EndCombo()
  end

  -- Combo box to choose component type + add / remove buttons
  if imgui.BeginCombo(self.entity_creator.comp_cbox_id, self.entity_creator.selected_comp) then
	for name, _ in pairs(tdengine.component_types) do
	  if imgui.Selectable(name) then
		self.entity_creator.selected_comp = name
	  end
	end
	imgui.EndCombo()
  end

  imgui.SameLine()
  if imgui.Button('Add') then
	local component = {}
	self.entity_creator.components[self.entity_creator.selected_comp] = component
	local editor = imgui.extensions.TableEditor(component)
	self.component_editors[self.entity_creator.selected_comp] = editor
  end

  imgui.SameLine()
  if imgui.Button('Remove') then
	self.entity_creator.components[self.entity_creator.selected_comp] = nil
	self.component_editors[self.entity_creator.selected_comp] = nil
  end

  -- Allow adding custom fields to the component
  for name, component in pairs(self.entity_creator.components) do
	-- Components can create other components, and can generally come from places besides
	-- the editor. So make sure they're all in here before we try to draw em.
	if not self.component_editors[name] then
	  self.component_editors[name] = imgui.extensions.TableEditor(component)
	end
	
	if imgui.TreeNode(name) then
	  -- Special, component specific stuff
	  if name == 'BoundingBox' then
		if imgui.Button('Draw Bounding Box') then
		  self.state = EditState.ReadyToDrawGeometry
		  self.use_geometry = true
		end
	  end

	  -- Then, just draw the component as a editable table
	  local component_editor = self.component_editors[name]
	  component_editor:draw()
	  
	  imgui.TreePop()
	end
  end

  if imgui.TreeNode('params') then
	self.param_editor:draw()
	imgui.TreePop()
  end

  if imgui.Button('Add Entity') then
	-- If we drew a bounding box for the entity, pull that data and then reset it
	if self.use_geometry then
	  if not self.entity_creator.components.BoundingBox then
		self.entity_creator.components.BoundingBox = {}
	  end
	  self.entity_creator.components.BoundingBox.extents = self.geometry.extents
	  self.entity_creator.components.BoundingBox.offset = tdengine.vec2(0, 0)

	  -- Only use the position from the box if no other position was specified
	  if not self.entity_creator.components.Position then
		self.entity_creator.components.Position = {}
		self.entity_creator.components.Position.world = self.geometry.origin
	  end

	  self.use_geometry = false
	  self.geometry = {
		origin = tdengine.vec2(0, 0),
		extents = tdengine.vec2(0, 0)
	  }
	end

	-- Build up all the entity's data into one table
	local data = {
	  name = self.entity_creator.selected_entity,
	  components = self.entity_creator.components,
	  params = self.entity_creator.params
	}

	-- Create the entity and load it with the data we just made
	local id = tdengine.create_entity(data.name, data)
	if id then
	  self.entity_creator.components = {}
	  self.component_editors = {}

	  self.param_editor:clear()
	  -- Clear out the params like this so the underlying reference is still the same
	  -- between the editor and the actual param table
	  for key, value in pairs(self.entity_creator.params) do
		self.entity_creator.params[key] = nil
	  end
	else
	  self.entity_creator.show_error = true
	end
  end
  
  if self.entity_creator.show_error then
	if imgui.Button('Error creating entity! Click to dismiss.') then
	  self.entity_creator.show_error = false
	end
  end
end

function Editor:draw_entity_viewer()
  self.filter:Draw("Filter by name")
  local entity_hovered_in_list = nil

  local sorted_ids = {}
  for id, entity in pairs(tdengine.entities) do
	table.insert(sorted_ids, id)
  end

  local compare_entity_ids = function(a, b)
	local entity_a = tdengine.entities[a]
	local entity_b = tdengine.entities[b]

	local name_a = entity_a:get_name()
	local name_b = entity_b:get_name()
	if name_a == name_b then
	  return entity_a.id < entity_b.id
	else
	  return name_a < name_b
	end
  end
  table.sort(sorted_ids, compare_entity_ids)
  
  for index, id in pairs(sorted_ids) do
	local entity = tdengine.entities[id]
	local name = entity:get_name()
	local sid = tostring(id)
	local tag = entity.tag

	local pass = false
	pass = pass or self.filter:PassFilter(name)
	pass = pass or self.filter:PassFilter(sid)
	pass = pass or (tag and self.filter:PassFilter(tag))
	if pass then
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
		self:select_entity(entity)
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

function Editor:draw_selected_entity()
  if self.selected ~= nil then
	imgui.Text(self.selected:get_name())
	self.entity_editor:draw()
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

  local button_size = { x = 100, y = 0 }
  local loaded = false
  
  local id = '##scene_viewer:save_to_template'
  if imgui.Button('Save Scene', button_size.x, button_size.y) then
	tdengine.save_current_scene_as_template(tdengine.loaded_scene.name)
  end
  
  local id = '##scene_viewer:load_from_template'
  if imgui.Button('Load Scene', button_size.x, button_size.y) then
	tdengine.unload_current_scene()
	tdengine.load_scene_template(imgui.InputTextContents(id))
	loaded = true
  end
  imgui.SameLine()
  imgui.InputText(id)

  if loaded then self:select_entity(nil) end

  imgui.extensions.WhitespaceSeparator(10)

  self:draw_entity_creator()
  imgui.extensions.WhitespaceSeparator(10)
  self:draw_entity_viewer()
  imgui.extensions.WhitespaceSeparator(10)
  self:draw_selected_entity()
  
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
	node.who = 'unknown'
  elseif kind == 'Choice' then
	node.text = ''
  elseif kind == 'Set' then
	node.variable = 'buns'
	node.value = true
  elseif kind == 'Empty' then
	node.internal_name = 'Empty'
  elseif kind == 'Branch' then
	node.branch_on = 'buns'
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
	
	tdengine.log('ded:@no_gui_layout:' .. filepath)
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
  local max_size = 16
  if node.kind == 'Text' or node.kind == 'Choice' then
	if string.len(node.text) < max_size then
	  return string.sub(node.text, 0, max_size)
	else
	  return string.sub(node.text, 0, max_size - 3) .. '...'
	end
  elseif node.kind == 'Set' then
	 return node.variable .. ' = ' .. tostring(node.value)
  elseif node.kind == 'Empty' then
	 return node.internal_name
  elseif node.kind == 'Branch' then
	 return node.branch_on
  else
	print('Editor:ded_short_text(): missing entry: ' .. node.kind)
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

  if node.kind == 'Empty' then
	imgui.InputTextSetContents(self.ded.empty_name_id, node.internal_name)
  end

  if node.kind == 'Text' then
	imgui.InputTextSetContents(self.ded.text_who_id, node.who)
  end
  
  if node.kind == 'Branch' then
	imgui.InputTextSetContents(self.ded.branch_on_id, node.branch_on)
  end
  
  if node.kind == 'Set' then
	imgui.InputTextSetContents(self.ded.set_var_id, node.variable)
	imgui.InputTextSetContents(self.ded.set_val_id, tostring(node.value))
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

function Editor:dialogue_editor(dt)
  imgui.Begin('dialogue', true)

  -- Draw the sidebar
  imgui.BeginChild('sidebar', 350, 0)
  
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
  imgui.InputText(id)

  id = '##ded_load'
  if imgui.Button('Load', button_size.x, button_size.y) then
	-- Because you can still click this and have the grid hidden
	tdengine.layout('ded') 

	self:ded_load(imgui.InputTextContents(id))
	imgui.InputTextClear(id)
  end
  imgui.SameLine()
  imgui.InputText(id)

  id = '##ded_new'
  if imgui.Button('New', button_size.x, button_size.y) then
	-- Because you can still click this and have the grid hidden
	tdengine.layout('ded') 

	self:ded_new(imgui.InputTextContents(id))
	imgui.InputTextClear(id)
  end
  imgui.SameLine()
  imgui.InputText(id)

  if imgui.Button('Try It!', button_size.x, button_size.y) then
	 tdengine.layout('tiny')

	 -- @hack: This works for this simple cutscene, but there will probably be
	 -- more complex cutscenes in the future that need proper teardown to be
	 -- idempotent...
	 local text_box = tdengine.find_entity('TextBox')
	 if text_box then
		tdengine.destroy_entity(text_box.id)
	 end
	 
	 local cutscene = {
		{
		   name = 'Dialogue',
		   dialogue = self.ded.loaded
		}
	 }
	 tdengine.do_cutscene_from_data(cutscene)
  end

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
	  imgui.InputText(self.ded.text_who_id, 64)

	  selected.who = imgui.InputTextContents(self.ded.text_who_id)
	end

	if selected.kind == 'Set' then
	  imgui.extensions.VariableName('state')
	  imgui.SameLine()
	  if imgui.InputText(self.ded.set_var_id, 64) then
		selected.variable = imgui.InputTextContents(self.ded.set_var_id)
	  end

	  imgui.extensions.VariableName('value')
	  imgui.SameLine()
	  if imgui.InputText(self.ded.set_val_id, 64) then
		local value = imgui.InputTextContents(self.ded.set_val_id)
		if value == 'true' then selected.value = true 
		elseif value == 'false' then selected.value = false end
	  end
	end

	if selected.kind == 'Branch' then
	   imgui.extensions.VariableName('branch on')
	   imgui.SameLine()

	   if imgui.InputText(self.ded.branch_on_id, 64) then
		  selected.branch_on = imgui.InputTextContents(self.ded.branch_on_id)
	   end
	end

	if selected.kind == 'Empty' then
	  imgui.extensions.VariableName('internal name')
	  imgui.SameLine()
	  imgui.InputText(self.ded.empty_name_id, 64)

	  selected.internal_name = imgui.InputTextContents(self.ded.empty_name_id)
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
	if node.kind == 'Text' then
	  imgui.Text(node.who)
	else
	  imgui.Text(node.kind)
	end
	imgui.Text(self:ded_short_text(node))
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
  local backlink_color = tdengine.color32(200, 100, 100, 255)
  local disconnect_color = tdengine.color32(255, 0, 0, 255)
  local thickness = 2

  for id, node in pairs(self.ded.nodes) do
	local output_slot = self:output_slot(id)
	local use_dc_prompt_color = self.ded.disconnecting == id
	
	local children = node.children
	for index, child_id in pairs(children) do
	  local input_slot = self:input_slot(child_id)

	  use_dc_prompt_color = use_dc_prompt_color and self.hovered == child_id

	  -- The graph looks super messy if we hook nodes later in the graph
	  -- to earlier nodes. So, instead of drawing the normal link, do a big
	  -- curve that swings around the whole graph (use the average of the two
	  -- slots' X coordinates for the peak)
	  if input_slot.x < output_slot.x then
		 local color = ternary(use_dc_prompt_color, disconnect_color, backlink_color)
		 local average = output_slot:subtract(input_slot):scale(.5)
		 imgui.DrawList_AddBezierCurve(
			output_slot.x, output_slot.y,
			output_slot.x - average.x, output_slot.y - 500,
			input_slot.x, input_slot.y,
			input_slot.x, input_slot.y,
			color, thickness)
	  else
		local color = ternary(use_dc_prompt_color, disconnect_color, link_color)
		local cp = tdengine.vec2(50, 0)
		imgui.DrawList_AddBezierCurve(
		  output_slot.x, output_slot.y,
		  output_slot.x + cp.x, output_slot.y,
		  input_slot.x - cp.x, input_slot.y,
		  input_slot.x, input_slot.y,
		  color, thickness)
	  end

	  -- To keep track of which branch node you will branch to depending on true / false /
	  -- index returned by branch function, label them.
	  if node.kind == 'Branch' then
		local middle = output_slot:add(input_slot):scale(.5)
		local colors = {
		  default = tdengine.color32(0, 255, 128, 255),
		  t = tdengine.color32(0, 255, 128, 255),
		  f = tdengine.color32(255, 0, 128, 255)
		}

		-- If there's more than two choices, we want to give it a number matching its index
		if #children > 2 then
		  -- Where the top left of the index glyph goes for >2 choices
		  local index_glyph_offset = tdengine.vec2(10, 15)
		  local index_glyph = middle:subtract(index_glyph_offset)

		  imgui.SetWindowFontScale(1.5)
		  imgui.SetCursorScreenPos(index_glyph:unpack())
		  imgui.PushStyleColor(imgui.constant.Col.Text, colors.default)
		  imgui.Text(tostring(index))
		  imgui.PopStyleColor()
		  imgui.SetWindowFontScale(1)
		-- If there are only two choices, label them with T/F dots
		elseif #children == 2 then		
		  local extents = tdengine.vec2(5, 5)
		  local min = middle:subtract(extents)
		  local max = middle:add(extents)
		  local color = ternary(index == 1, colors.t, colors.f)
		  local rounding = 50
		  imgui.DrawList_AddRectFilled(min.x, min.y, max.x, max.y, color, rounding)
		end
	  end
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
	  if imgui.MenuItem('Empty') then
		node = self:make_dialogue_node('Empty')
	  end
	  if imgui.MenuItem('Branch') then
		node = self:make_dialogue_node('Branch')
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

  if canvas_hovered and not clicked_on_node then
	 if middle_click then
		local delta = tdengine.vec2(imgui.MouseDelta())
		self.ded.scrolling = self.ded.scrolling:add(delta)
	 end

	 local input = self:get_component('Input')
	 input:set_channel(tdengine.InputChannel.ImGui)

	 self.ded.scroll_per_second = 1000
	 local delta = tdengine.vec2(0, 0)
 	 if input:is_down(GLFW.Keys.W) then
		delta.y = delta.y + (self.ded.scroll_per_second * dt)
	 end
 	 if input:is_down(GLFW.Keys.S) then
		delta.y = delta.y - (self.ded.scroll_per_second * dt)
	 end
 	 if input:is_down(GLFW.Keys.A) then
		delta.x = delta.x + (self.ded.scroll_per_second * dt)
	 end
 	 if input:is_down(GLFW.Keys.D) then
		delta.x = delta.x - (self.ded.scroll_per_second * dt)
	 end
	 
	 input:set_channel(tdengine.InputChannel.Editor)

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
	  imgui.extensions.VariableName(name)
	  imgui.SameLine()

	  local value = tdengine.state[name]
	  if type(value) == 'string' then
		imgui.Text(value)
	  elseif type(value) == 'boolean' then
		local true_color = tdengine.color32(0, 255, 0, 255)
		local false_color = tdengine.color32(255, 0, 0, 255)
		local color = ternary(value, true_color, false_color)
		imgui.PushStyleColor(imgui.constant.Col.Text, color)
		
		local label = tostring(tdengine.state[name]) .. '##' .. tostring(index)
		if imgui.Button(label) then
		  tdengine.state[name] = not value
		end
	  
		imgui.PopStyleColor()
	  elseif type(value) == 'number' then
		imgui.Text(tostring(value))
	  end
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
  if imgui.Button('Terminate') then
	tdengine.terminate_cutscene()
  end

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

function Editor:toggle_follow()
   self.following_player = not self.following_player
   tdengine.follow_player(self.following_player)
end

function Editor:select_entity(entity)
  self.selected = entity
  if entity then
	self.entity_editor = imgui.extensions.EntityEditor(self.selected)
  end
end
