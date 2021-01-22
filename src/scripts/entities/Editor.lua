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
