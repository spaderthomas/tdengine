Move = tdengine.create_class('Move')
tdengine.add_new_to_class(Move, _G)

function Move:init(name)
  self.name = name
  self.data = tdengine.fetch_module_data('battle/moves/' .. name)
end

function Move:static_data()
  return tdengine.fetch_module_data('battle/moves/' .. self.name)
end

function Move:flavor_name()
  return self.data.flavor_name
end
