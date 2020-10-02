local GLFW = require('glfw')
local inspect = require('inspect')

local EditState = {
  Idle = 0,
  Insert = 1,
  Edit = 2,
  Drag = 3,
  RectangleSelect = 4,
}

local SelectionState = {
  None = 0,
  Tile = 1,
  Entity = 2
}

Editor = tdengine.entity('Editor')
function Editor:init()
  self:add_component('Debug')
  self:add_component('Input')

  self.options = {
	 show_bounding_boxes = false,
	 show_minkowksi = false,
	 show_imgui_demo = false,
	 show_framerate = false
  }
  
  self.selected = nil
  self.state = EditState.Idle
  self.selection_state = SelectionState.None
  
  self.snap = false
  self.last_snap = false
  self.show_grid = false
  self.last_show_grid = false

  self.filter = imgui.TextFilter.new()

  self.display_framerate = 0
  self.average_framerate = 0
  self.frame = 0

  local input = self:get_component('Input')
  input:set_channel(tdengine.InputChannel.Editor)
  input:enable()
end

function Editor:update(dt)
  local dbg = self:get_component('Debug')
  local input = self:get_component('Input')
  local player = self:get_entity('Player')
  self:calculate_framerate()
  
  self:handle_input()
  if self.show_grid then self:draw_grid() end
  
  imgui.SetNextWindowSize(300, 300)
  imgui.Begin("tded v2.0", true)
  imgui.Text('frame: ' .. tostring(self.frame))
  imgui.Text('fps: ' .. tostring(self.display_framerate))
  self:draw_entity_viewer()
  imgui.End()
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
  if input:was_key_pressed(GLFW.Keys.ESCAPE) then
	print('esc')
	self.selected = nil
	self.state = EditState.Idle

	-- If you were selecting a tile and escaped, bring the state back to how it was
	if self.selection_state == SelectionState.Tile then
	  self.snap = self.last_snap
	  self.show_grid = self.last_show_grid
	end
  end

  if input:was_key_pressed(GLFW.Keys.LEFT_CONTROL) then
    tdengine.internal.toggle_console()
  end
end

function Editor:adjust_camera()
  local input = self:get_component('Input')

  local offset = { x = 0, y = 0 }
  if input:is_key_down(GLFW.Keys.W) then
	offset.y = offset.y - .02
  end
  
  if input:is_key_down(GLFW.Keys.A) then
	offset.x = offset.x + .02
  end
  
  if input:is_key_down(GLFW.Keys.S) then
	offset.y = offset.y + .02
  end
  
  if input:is_key_down(GLFW.Keys.D) then
	offset.x = offset.x - .02
  end

  tdengine.move_camera(offset.x, offset.y)
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

function Editor:draw_grid()
   local tsx = tdengine.Units.TileSize.Screen.x
   local tsy = tdengine.Units.TileSize.Screen.y
   local xb = math.fmod(tdengine.get_camera_x(), tsx)
   local yb = math.fmod(tdengine.get_camera_y(), tsy) + (tsy / 2)

   for col_offset = 1 - xb, 0, -tsx do
	  tdengine.internal.draw_line_from_points(col_offset, 0, col_offset, 1, .2, .1, .9, .5)
   end

   for row_offset = 1 - yb, 0, -tsy do
	  tdengine.internal.draw_line_from_points(0, row_offset, 1, row_offset, .2, .1, .9, .5)
   end
end
