-- ImGui extensions
imgui.extensions = {}

imgui.extensions.Entity = function(entity)
   ignore = {
	  class = true
   }
   imgui.extensions.Table(entity:get_name(), entity, ignore)
end
imgui.extensions.Table = function(name, tbl, ignore)
   ignore = ignore or {}
   
   if imgui.TreeNode(name) then
	  for member, value in pairs(tbl) do
		 if ignore[member] == nil then
			local value_type = type(value)
			if value_type == 'string' then  
			   imgui.Text(member .. ': ')
			   imgui.SameLine()
			   imgui.Text(value)
			elseif value_type == 'number' then
			   imgui.Text(member .. ': ')
			   imgui.SameLine()
			   imgui.Text(tostring(value))
			elseif value_type == 'boolean' then
			   imgui.Text(member)
			   imgui.SameLine()
			   unique_imgui_id = '##' .. member
			   draw, new = imgui.Checkbox(unique_imgui_id, value)
			   tbl[member] = new
			elseif value_type == 'table' then
			   imgui.extensions.Table(member, value)
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
