local inspect = require('inspect')

-- ImGui extensions
imgui.extensions = imgui.extensions or {}

imgui.extensions.Component = function(component)
   imgui.PushID(component:get_id())
   
   if imgui.TreeNode(component:get_name()) then
	  imgui.extensions.VariableName('id')
	  imgui.SameLine()
	  imgui.Text(tostring(component:get_id()))
	  imgui.extensions.TableMembers(component:get_name(), component)
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

	  imgui.extensions.TableMembers(entity:get_name(), entity)
	  imgui.TreePop()
   end

   imgui.PopID()
end

imgui.extensions.TableMembers = function(name, tbl, ignore, imgui_id)
   ignore = tbl.imgui_ignore or {}
   imgui_id = imgui_id or '##' .. name
   for member, value in pairs(tbl) do
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

imgui.extensions.Table = function(name, tbl, ignore, imgui_id)
   ignore = ignore or {}
   imgui_id = imgui_id or '##' .. name
   
   if imgui.TreeNode(name) then
	  imgui.extensions.TableMembers(name, tbl, ignore, imgui_id)
	  imgui.TreePop()
   end
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
   imgui.PushStyleColor_2(imgui.constant.Col.Text, .1, .2, .7, 1)
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
