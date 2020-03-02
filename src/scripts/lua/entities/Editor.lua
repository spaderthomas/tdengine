local GLFW = require('glfw')
local inspect = require('inspect')

local EDITOR_STATE = {
  idle = 0,
  insert = 1,
  edit = 2,
  drag = 3,
  rectangle_select = 4
}

local SELECTION_STATE = {
  none = 0,
  tile = 1,
  entity = 2
}

Editor = tdengine.entity('Editor')
function Editor:init()
  self:add_component('Camera')
  self:add_component('Input')

  self.selected = nil
  self.state = EDITOR_STATE.idle
  self.selection_state = SELECTION_STATE.none
  
  self.snap = false
  self.last_snap = false
  self.show_grid = false
  self.last_show_grid = false

  print(inspect(imgui))
end

function Editor:update(dt)
  self:handle_input()
  --imgui.SetNextWindowSize(300, 300)
  imgui.Begin("Shit", true)
  imgui.Button("hello", 40, 40)
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
