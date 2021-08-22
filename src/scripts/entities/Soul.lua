Soul = tdengine.entity('Soul')

function Soul:init(params)
  self.data = params
end

function Soul:update(dt)
end

function Soul:get_static_data()
  return tdengine.fetch_module_data('battle/souls/' .. self.data.name)
end

function Soul:get_moves()
  local moves = {}
  for index, move_name in pairs(self.data.moves) do
	table.insert(moves, tdengine.fetch_module_data('battle/moves/' .. move_name))
  end
  return moves
end

function Soul:get_soul_name()
  return self.data.name
end
