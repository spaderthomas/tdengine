local inspect = require('inspect')

-- ImGui extensions
imgui.extensions = imgui.extensions or {}

imgui.extensions.Entity = function(entity)
   imgui.PushID(entity:get_id())
   imgui.extensions.Table(entity:get_name(), entity, entity.imgui_ignore, ignore)
   imgui.PopID()
end

imgui.extensions.Table = function(name, tbl, ignore, imgui_id)
   ignore = ignore or {}
   imgui_id = imgui_id or '##' .. name
   
   if imgui.TreeNode(name) then
	  for member, value in pairs(tbl) do
		 if not ignore[member] then
			local value_type = type(value)
			local next_imgui_id = imgui_id .. '__' .. member
			
			if value_type == 'string' then
			   imgui.extensions.VariableName(member)
			   imgui.extensions.RightAlignedString(value)
			elseif value_type == 'number' then
			   imgui.extensions.VariableName(member)
			   imgui.extensions.RightAlignedString(tostring(value))
			elseif value_type == 'boolean' then
			   imgui.extensions.VariableName(member)
			   imgui.SameLine(imgui.GetWindowWidth() - 30)
			   draw, new = imgui.Checkbox(next_imgui_id, value)
			   tbl[member] = new
			elseif value_type == 'table' then
			   imgui.extensions.Table(member, value, {}, next_imgui_id)
			end
		 end
	  end
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
   imgui.Text(name)
   imgui.PopStyleColor()
   imgui.SameLine()
   imgui.Text(': ')
end

imgui.extensions.RightAlignedString = function(str)
   local width = imgui.GetWindowWidth()
   local text_size = imgui.CalcTextSize(str)
   local padding = 10
   imgui.SameLine(width - text_size - padding)
   imgui.Text(str)
end
