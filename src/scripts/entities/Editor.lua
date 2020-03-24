local GLFW = require('glfw')
local inspect = require('inspect')

local EDITOR_STATE = {
  idle = 0,
  insert = 1,
  edit = 2,
  drag = 3,
  rectangle_select = 4,
}

local SELECTION_STATE = {
  none = 0,
  tile = 1,
  entity = 2
}

Editor = tdengine.entity('Editor')
function Editor:init()
  self:add_component('Camera')
  self:add_component('Debug')
  self:add_component('Input')

  self.options = {
	 show_bounding_boxes = false,
	 show_minkowksi = false,
	 show_imgui_demo = false,
	 show_framerate = false
  }
  
  self.selected = nil
  self.state = EDITOR_STATE.idle
  self.selection_state = SELECTION_STATE.none
  
  self.snap = false
  self.last_snap = false
  self.show_grid = false
  self.last_show_grid = false

  self.filter = imgui.TextFilter.new()

  self.entity_tree = {}

  local input = self:get_component('Input')
  input:set_channel(tdengine.InputChannel.Editor)
  input:enable()

  self:add_imgui_ignore('entity_tree')
  self:add_imgui_ignore('outer_heaven')
end

function Editor:update(dt)
  local dbg = self:get_component('Debug')

  local input = self:get_component('Input')

  self:handle_input()
  imgui.SetNextWindowSize(300, 300)
  imgui.Begin("tded v2.0", true)
  self:draw_entity_viewer()
  self:draw_options()
  imgui.End()
end

function Editor:handle_input()
  self:adjust_camera()
  
  local input = self:get_component('Input')
  if input:was_key_pressed(GLFW.Keys.ESCAPE) then
	print('esc')
	self.selected = nil
	self.state = EDITOR_STATE.idle

	if self.selection_state == SELECTION_STATE.tile then
	  self.snap = self.last_snap
	  self.show_grid = self.last_show_grid
	end
  end
end

function Editor:adjust_camera()
  local camera = self:get_component('Camera')
  local input = self:get_component('Input')

  local offset = { x = 0, y = 0 }
  if input:is_key_down(GLFW.Keys.W) then
	offset.y = offset.y + .025
  end
  
  if input:is_key_down(GLFW.Keys.A) then
	offset.x = offset.x - .025
  end
  
  if input:is_key_down(GLFW.Keys.S) then
	offset.y = offset.y - .025
  end
  
  if input:is_key_down(GLFW.Keys.D) then
	offset.x = offset.x + .025
  end

  camera.offset.x = camera.offset.x + offset.x
  camera.offset.y = camera.offset.y + offset.y
end

function Editor:draw_entity_viewer()
  imgui.Begin("scene", true)

  self.filter:Draw("Filter by name")
  for id, entity in pairs(Entities) do
	local name = entity:get_name()
	if self.filter:PassFilter(name) then
	  imgui.extensions.Entity(entity)
	end
  end
  
  imgui.End()
end

function Editor:draw_options()
   imgui.Begin('options', true)
   for option, value in pairs(self.options) do
	  draw, new_value = imgui.Checkbox(option, value)
	  self.options[option] = new_value
   end
   imgui.End()
end

