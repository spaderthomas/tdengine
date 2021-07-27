local inspect = require('inspect')

-- ImGui extensions
imgui.extensions = imgui.extensions or {}
imgui.internal = imgui.internal or {}

imgui.extensions.Component = function(component)
   imgui.PushID(component:get_id())
   
   if imgui.TreeNode(component:get_name()) then
	  imgui.extensions.VariableName('id')
	  imgui.SameLine()
	  imgui.Text(tostring(component:get_id()))

	  local name = component:get_name()
	  local ignore = component.imgui_ignore
	  imgui.extensions.TableMembers(name, component, ignore)
	  imgui.TreePop()
   end

   imgui.PopID()
end

imgui.extensions.Entity = function(entity, unique_id)
   imgui.PushID(entity:get_id())
   local id = entity:get_name()

   if unique_id then id = id .. unique_id end

   if imgui.TreeNode(id) then
	  imgui.extensions.VariableName('id')
	  imgui.SameLine()
	  imgui.Text(tostring(entity:get_id()))

	  local components = entity:all_components()
	  for index, component in pairs(components) do
		 imgui.extensions.Component(component)
	  end

	  imgui.extensions.TableMembers(entity:get_name(), entity, entity.imgui_ignore)
	  imgui.TreePop()
   end

   imgui.PopID()
end

imgui.extensions.TableMembers = function(name, t, ignore, imgui_id)
   local ignore = ignore or {}
   imgui_id = imgui_id or '##' .. name
   for member, value in pairs(t) do
	  if not ignore[member] then
		 local value_type = type(value)
		 local next_imgui_id = imgui_id .. '__' .. member
		 
		 if value_type == 'string' then
			imgui.extensions.VariableName(member)
			imgui.SameLine()
			imgui.Text(value)
		 elseif value_type == 'number' then
			imgui.extensions.VariableName(member)
			imgui.SameLine()
			imgui.Text(tostring(value))
		 elseif value_type == 'boolean' then
			imgui.extensions.VariableName(member)
			imgui.SameLine()
			imgui.Text(tostring(value))
		 elseif value_type == 'table' then
			imgui.extensions.Table(member, value, {}, next_imgui_id)
		 end
	  end
   end
end


imgui.extensions.Table = function(name, t, ignore, imgui_id)
   ignore = ignore or {}
   imgui_id = imgui_id or '##' .. name
   name = name .. imgui_id
   
   if imgui.TreeNode(name) then
	  imgui.extensions.TableMembers(name, t, ignore, imgui_id)
	  imgui.TreePop()
   end
end

imgui.extensions.EditableTableMembers = function(t, ignore)
  if not t then return end
  
  local ignore = ignore or {}
end

local types = {
  'number',
  'string',
  'bool',
  'table'
}

imgui.internal.draw_table_editor = function(editor)
  imgui.extensions.VariableName('type')
  
  imgui.PushItemWidth(80)
  imgui.SameLine()
  if imgui.BeginCombo(editor.type_id, editor.selected_type) then
	for index, name in pairs(types) do
	  if imgui.Selectable(name) then
		editor.selected_type = name
	  end
	end
	imgui.EndCombo()
  end
  imgui.PopItemWidth()

  imgui.SameLine()
  imgui.extensions.VariableName('key')
  
  imgui.PushItemWidth(100)
  imgui.SameLine()
  local submit = imgui.InputText(editor.key_id)
  imgui.PopItemWidth()

  imgui.PushItemWidth(170)
  if editor.selected_type ~= 'table' then
	imgui.SameLine()
	imgui.extensions.VariableName('value')
  
	imgui.SameLine()
	submit = submit or imgui.InputText(editor.value_id)
  end
  imgui.PopItemWidth()
  

  if submit then
	local key = imgui.InputTextContents(editor.key_id)
	imgui.InputTextSetContents(editor.key_id, '')
	local value = imgui.InputTextContents(editor.value_id)
	imgui.InputTextSetContents(editor.value_id, '')
	
	if value == 'nil' then
	  value = nil
	elseif editor.selected_type == 'number' then
	  value = tonumber(value)
	elseif editor.selected_type == 'string' then
	  value = tostring(value)
	elseif editor.selected_type == 'bool' then
	  value = (value == 'true')
	elseif editor.selected_type == 'table' then
	  value = {}
	  editor.children[key] = imgui.extensions.TableEditor(value)
	end

	editor.editing[key] = value
	imgui.SetKeyboardFocusHere(-1)
  end

  for key, value in pairs(editor.editing) do
	if type(value) == 'string' then
	  imgui.extensions.VariableName(key)
	  imgui.SameLine()
	  imgui.Text(value)
	elseif type(value) == 'number' then
	  imgui.extensions.VariableName(key)
	  imgui.SameLine()
	  imgui.Text(tostring(value))
	elseif type(value) == 'boolean' then
	  imgui.extensions.VariableName(key)
	  imgui.SameLine()
	  imgui.Text(tostring(value))
	elseif type(value) == 'table' then
	  if not editor.children[key] then
		editor.children[key] = imgui.extensions.TableEditor(value)
	  end
	  local child = editor.children[key]
	  if imgui.TreeNode(key) then
		child:draw()
		imgui.TreePop()
	  end
	end
  end
end

imgui.internal.clear_table_editor = function(editor)
  editor.key_id = tdengine.uuid_imgui()
  editor.value_id = tdengine.uuid_imgui()
  editor.type_id = tdengine.uuid_imgui()
  editor.children = {}
end

imgui.extensions.TableEditor = function(editing)
  local editor = {
    key_id = tdengine.uuid_imgui(),
    value_id = tdengine.uuid_imgui(),
    type_id = tdengine.uuid_imgui(),
	selected_type = 'string',
	editing = editing,
	children = {},
	draw = function(self) imgui.internal.draw_table_editor(self) end,
	clear = function(self) imgui.internal.clear_table_editor(self) end
  }

  for key, value in pairs(editing) do
	if type(value) == 'table' then
	  editor.children[key] = imgui.extensions.TableEditor(value)
	end
  end

  return editor
end

imgui.extensions.PushBoolColor = function()
  imgui.PushStyleColor_2(imgui.constant.Col.Text, .9, .2, .7, 1)
end

imgui.extensions.PushStringColor = function()
  imgui.PushStyleColor_2(imgui.constant.Col.Text, .5, .2, .7, 1)
end

imgui.extensions.PushNumberColor = function()
  imgui.PushStyleColor_2(imgui.constant.Col.Text, .1, .2, .7, 1)
end

imgui.extensions.InputFloat = function(label, value, format)
   local step = 0
   local step_fast = 0
   local format = format or "%3.f"
   local extra_flags = 0
   return imgui.InputFloat(label, value, step, step_fast, format, extra_flags)
end

imgui.extensions.VariableName = function(name)
   local color = tdengine.color32(0, 200, 200, 255)
   imgui.PushStyleColor(imgui.constant.Col.Text, color)
   imgui.Text(name .. ':')
   imgui.PopStyleColor()
end

imgui.extensions.RightAlignedString = function(str)
   local width = imgui.GetWindowWidth()
   local text_size = imgui.CalcTextSize(str)
   local padding = 10
   imgui.SameLine(width - text_size - padding)
   imgui.Text(str)
end

imgui.extensions.Vec2 = function(name, v)
   if not name or not v then
	  print('vec2 missing a parameter')
	  return
   end
   imgui.Text(name .. ': (' .. tostring(v.x) .. ', ' .. tostring(v.y) .. ')')
end
