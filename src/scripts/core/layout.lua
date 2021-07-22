function tdengine.layout(name)
  tdengine.push_layout(tdengine.current_layout)
  tdengine.use_layout(name)
  tdengine.current_layout = name
end

function tdengine.push_layout(name)
  delete(tdengine.layout_stack, name)
  table.insert(tdengine.layout_stack, name)
end

function tdengine.next_layout()
  local stack_size = #tdengine.layout_stack
  local index = tdengine.layout_index
  tdengine.layout_index = (index % stack_size) + 1
  
  local layout = tdengine.layout_stack[tdengine.layout_index]
  tdengine.use_layout(layout)   
end


function tdengine.previous_layout()
  local stack_size = #tdengine.layout_stack
  local index = tdengine.layout_index - 2
  tdengine.layout_index = (index % stack_size) + 1
  
  local layout = tdengine.layout_stack[tdengine.layout_index]
  tdengine.use_layout(layout)   
end
