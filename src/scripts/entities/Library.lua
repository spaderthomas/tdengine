Library = tdengine.entity('Library')
function Library:init(params)
end

function Library:setup()
  local story_marker = tdengine.state['main:story_marker']
  self.destroy = {}

  -- Create entities
  local entities = Library.entities[story_marker]
  if entities then
	for index, entity in pairs(entities) do
	  local id = tdengine.create_entity(entity.name, entity)
	  table.insert(self.destroy, id)
	end
  end
end

function Library:cleanup()
  if not self.destroy then return end
  
  for index, id in pairs(self.destroy) do
	tdengine.destroy_entity(id)
  end
end

function Library:update(dt)
end
