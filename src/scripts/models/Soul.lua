Soul = tdengine.create_class('Soul')
tdengine.add_new_to_class(Soul, _G)

print(inspect(Soul))

function Soul:init(params)
  self.data = params
  self.static_data = tdengine.fetch_module_data('battle/souls/' .. self.data.name)
  self.fainted = false
end

function Soul:moves()
  local moves = {}
  for index, move_name in pairs(self.data.moves) do
	local move = Move:new()
	move:init(move_name)
	table.insert(moves, move)
  end
  return moves
end

function Soul:internal_name()
  return self.static_data.name
end

function Soul:flavor_name()
  return self.static_data.flavor_name
end

function Soul:front_sprite()
  return self.static_data.front_sprite
end
